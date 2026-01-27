#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include "lwp.h"
#include "fp.h"
#include "scheduler.h"

#define DEFAULT_STACK_SIZE (8 * 1024 * 1024) // 8 MB
// Definitions for waiting and terminated lists
#define next_waiting lib_one
#define next_terminated lib_two




// RULES
// saved bp/end of args must be divisible by 16


// TODO - move scheduler headers to otehr file
// TODO - make sure we're terminating the program correctly


// Yields control to another LWP, chosen by the scheduelr
// Saves current lwp context, picks next one and restores context
// if no next thread, terminates the program

// globals for our lwp system
static thread current_thread = NULL; // track current thread 
static thread oldest_waiting_thread_head = NULL; // LL of waiting theads
static thread oldest_terminated_thread_head = NULL; // LL of terminated threads
static int max_thread_id;




// Terminates the current lwp, yields to whichever thread the scheduler chooses
// Does not return
void lwp_exit(int exitval) {
    //  termination status becomes the low 8 bits of exitval
    current_thread->status = exitval & 0xFF;

    // deschedule current thread
    scheduler s = lwp_get_scheduler();
    s->remove(current_thread);

    // put current thread on terminated threads list
    if(oldest_terminated_thread_head == NULL) {
        oldest_terminated_thread_head=current_thread;
    }
    else {
        thread current = oldest_terminated_thread_head;
        while (current->next_terminated) {
            current = current->next_terminated;
        }
        current->next_terminated = current_thread;
        current_thread->next_terminated=NULL;
    }

    // reschedule a waiting thread if there is one
    if(oldest_waiting_thread_head != NULL) {
        thread rescheduling_thread = oldest_waiting_thread_head;
        oldest_waiting_thread_head = oldest_waiting_thread_head->next_waiting;
        s->admit(rescheduling_thread);
    }

    // yield to next thread
    lwp_yield();

    // Error - yield returns to lwp exit
    fprintf(stderr, "lwp_exit: fatal error, returned from lwp_yield\n");
    exit(1);
}



void lwp_yield(void) {
    // get next thread from scheduler
    scheduler s = lwp_get_scheduler();
    thread next_thread = s->next();

    printf("yielding to next thread: %lu\n", next_thread->tid);

    // if no next thread, terminate the program
    if (next_thread == NULL) {
        fprintf(stderr, "No next thread to switch to. Terminating.\n");
        exit(1); // call exit with termination status of calling thread
    }

    // save current thread and update it
    thread prev_current_thread = current_thread;
    current_thread = next_thread;

    // save current thread state, and transfer control
    swap_rfiles(&(prev_current_thread->state), &(current_thread->state));
    printf("Returning from yield\n");
    return; // pop return address into instruction pointer

}


// Starts the threading system. Converts original system thread into a LWP
void lwp_start(void) {
    max_thread_id = 0;

    // create a context for calling thread on the heap
    thread t = malloc(sizeof(struct threadinfo_st));
    t->tid = max_thread_id;
    t->sched_one = t;
    t->sched_two = t;
    t->stack = NULL;
    t->stacksize = 0;
    t->status = LWP_LIVE;
    t->exited = NULL;
    t->lib_one = NULL;
    t->lib_two = NULL;


    printf("swapping rfiles\n");
    // save current register state
    // swap_rfiles(&t->state, NULL);

    printf("setting scheduler\n");
    // admit calling thread to scheduler
    lwp_set_scheduler(NULL);
    scheduler s = lwp_get_scheduler();
    s->admit(t);

    // update current_thread
    current_thread = t;

    printf("yielding\n");
    // yield to next thread in schedule
    lwp_yield();

    // yielded back to main thread
    printf("Thread yielded control\n");
    return;
}

// helper --  determine stack size for allocation
size_t get_stack_size() {
    // get page size
    long pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize <= 0) {
        perror("sysconf(_SC_PAGESIZE)");
        return DEFAULT_STACK_SIZE;  // fallback
    }

    
    // check stack size limit, and use if not infinity
    struct rlimit rl;
    if (getrlimit(RLIMIT_STACK, &rl) != 0) {
        perror("getrlimit");
        return DEFAULT_STACK_SIZE;
    }

    size_t stack_size;
    if (rl.rlim_cur == RLIM_INFINITY) {
        stack_size = DEFAULT_STACK_SIZE;
    } else {
        stack_size = rl.rlim_cur;
    }

    // round up to multiple of page size
    stack_size = ((stack_size + pagesize - 1) / pagesize) * pagesize;

    return stack_size;
}

tid_t lwp_create(lwpfun function, void *argument) {
    max_thread_id++;
    printf("creating thread %d\n", max_thread_id);


    // Initialize new thread's stack frame
    size_t stack_size = get_stack_size();
    void *stack_ptr = mmap(NULL,stack_size,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK,-1,0);
    if (stack_ptr == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    void *stack_bottom = (char *)stack_ptr + stack_size;

    // Initialize new thread's context
    thread t = malloc(sizeof(struct threadinfo_st));
    if (!t) {
        perror("malloc");
        return -1;
    }
    t->tid = max_thread_id;
    t->sched_one = t;
    t->sched_two = t;
    t->stack = stack_ptr; // base of mmap
    t->stacksize = stack_size;
    t->status = LWP_LIVE;
    t->exited = NULL;
    t->lib_one = NULL;
    t->lib_two = NULL;

    // set up stack for swaprfiles to teardown
    uintptr_t *sp = (uintptr_t *)stack_bottom;

    --sp; // spot for return address
    *sp = (uintptr_t)function;

    --sp;            // slot for old rbp
    *sp = 0;         // fake frame pointer
    t->state.rbp = (unsigned long)sp;


    // admit new thread to the schedule
    scheduler s = lwp_get_scheduler();
    s->admit(t);
    // schedule length
    int length = s->qlen();
    printf("Current schedule length: %d\n", length);
    printf("returning from lwp_create\n");

    return t->tid;
}


// deallocates the resources of next terminated thread
// blocks until the thread terminates, returns NO_THREAD if none left to wait for
tid_t lwp_wait(int *status) {
    // maintain list of waiting threads
    // maintain list of terminated threads

    scheduler s = lwp_get_scheduler();

    // loop - run until we successfully deschedule or fail
    while (1) {
        // check if there are terminated threads
        if(oldest_terminated_thread_head) {
            // deallocate oldest one
            thread deallocate_thread = oldest_terminated_thread_head;

            // update LL
            thread next_thread = oldest_terminated_thread_head->next_terminated;
            if(next_thread) {
                oldest_terminated_thread_head = next_thread;
            }
            else { 
                // no more after
                oldest_terminated_thread_head = NULL;
            }
            
            int deallocate_id = deallocate_thread->tid;
            // update status
            if (status != NULL) {
                *status = deallocate_thread->status;
            }
            // deallocate thread's stack
            munmap(deallocate_thread->stack, deallocate_thread->stacksize);
            // free thread struct
            free(deallocate_thread);
            return deallocate_id;
        }

        // no terminated threads waiting --
        if (s->qlen() <= 0) { // check if there are more threads
            return NO_THREAD;
        }

        // blocking -- put current thread on waiting list
        if (oldest_waiting_thread_head == NULL) {
            oldest_waiting_thread_head = current_thread;
            current_thread->next_waiting=NULL;
        }
        else {
            thread current = oldest_waiting_thread_head
            while (current->next_waiting) {
                current = current->next_waiting;
            }
            current->next_waiting=current_thread;
            current_thread->next_waiting=NULL;
        }

        // deschedule and yield
        s->remove(current_thread);
        lwp_yield();
    }


}


tid_t lw_gettid(void);


// static void lwp_wrap(lwpfun fun, void *arg) {
//     /*
//     call the given lwp function with the given argument
//     calls lwp_exit() with its return value
//     */
//     int rval;
//     rval=fun(arg);
//     lwp_exit(rval);
// }