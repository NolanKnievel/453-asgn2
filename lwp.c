#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include "lwp.h"
#include "fp.h"
#include "scheduler.h"

#define DEFAULT_STACK_SIZE (8 * 1024 * 1024) // 8 MB



// RULES
// saved bp/end of args must be divisible by 16
// TODO - move scheduler headers to otehr file
// TODO - make sure we're terminating the program correctly


// Yields control to another LWP, chosen by the scheduelr
// Saves current lwp context, picks next one and restores context
// if no next thread, terminates the program

// track current thread globally -- 
static thread current_thread = NULL;


// Terminates the current lwp, yields to whichever thread teh scheduler chooses
// Does not return
void lwp_exit(int exitval);



void lwp_yield(void) {
    // get next thread from scheduler
    scheduler s = lwp_get_scheduler();
    thread next_thread = s->next();
    printf("Next thread: %lu\n", next_thread->tid);

    // if no next thread, terminate the program
    if (next_thread == NULL) {
        fprintf(stderr, "No next thread to switch to. Terminating.\n");
        exit(1); // call exit with termination status of calling thread
    }

    // save current thread and update it
    thread prev_current_thread = current_thread;
    current_thread = next_thread;

    printf("Swapping rfiles\n");
    // save current thread state, and transfer control
    swap_rfiles(&(prev_current_thread->state), &(current_thread->state));
    printf("Returning\n");
    return; // pop return address into instruction pointer

}


// Starts the threading system. Converts original system thread into a LWP
void lwp_start(void) {
    
    // create a context for calling thread on the heap
    thread t = malloc(sizeof(struct threadinfo_st));
    t->tid = 0;
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
    // Initialize new thread's stack frame
    size_t stack_size = get_stack_size();
    void *stack_ptr = mmap(NULL,stack_size,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK,-1,0);
    if (stack_ptr == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    void *stack_top = (char *)stack_ptr + stack_size;

    printf("top of stack: %p\n", stack_top);

    // Initialize new thread's context



    // admit new thread to the schedule
    return 0;
}


tid_t lwp_wait(int *status);
tid_t lw_gettid(void);