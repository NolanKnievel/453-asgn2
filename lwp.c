#include <sys/user.h>
#include <stdlib.h>
#include <stdio.h>
#include "lwp.h"
#include "fp.h"
#include "scheduler.h"

// RULES
// saved bp/end of args must be divisible by 16
// TODO - move scheduler headers to otehr file
// TODO - make sure we're terminating the program correctly


// Yields control to another LWP, chosen by the scheduelr
// Saves current lwp context, picks next one and restores context
// if no next thread, terminates the program

// track current thread globally -- 
static thread current_thread = NULL;

// debugging struct
struct user_regs_struct regs;


// Terminates the current lwp, yields to whichever thread teh scheduler chooses
// Does not return
void lwp_exit(int exitval);



void lwp_yield(void) {
    // get next thread from scheduler
    scheduler s = lwp_get_scheduler();
    thread next_thread = s->next();

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
    return; // pop return address into instruction pointer

}


// Starts the threading system. Converts original system thread into a LWP
void lwp_start(void) {
    
    // create context for calling thread
    struct threadinfo_st t;
    t.tid = 0;
    t.sched_one = &t;
    t.sched_two = &t;
    t.stack = NULL;
    t.stacksize = 0;
    t.status = LWP_LIVE;
    t.exited = NULL;
    t.lib_one = NULL;
    t.lib_two = NULL;

    

    // debug - log regs
    if (inforeg(pid, &regs) == 0) {  // assume 0 = success
        printf("RAX: 0x%llx\n", regs.rax);
        printf("RBX: 0x%llx\n", regs.rbx);
        printf("RIP: 0x%llx\n", regs.rip);   // instruction pointer
        printf("RSP: 0x%llx\n", regs.rsp);   // stack pointer
    }


    // save current register state
    swap_rfiles(&t.state, NULL);

    // debug - log regs
    if (inforeg(pid, &regs) == 0) {  // assume 0 = success
        printf("RAX: 0x%llx\n", regs.rax);
        printf("RBX: 0x%llx\n", regs.rbx);
        printf("RIP: 0x%llx\n", regs.rip);   // instruction pointer
        printf("RSP: 0x%llx\n", regs.rsp);   // stack pointer
    }

    
    // admit calling thread to scheduler
    lwp_set_scheduler(NULL);
    scheduler s = lwp_get_scheduler();
    s->admit(&t);

    // yield to next thread in schedule
    lwp_yield();

    printf("Thread yielded control\n");
    return;
}


tid_t lwp_create(lwpfun function, void *argument);
tid_t lwp_wait(int *status);
tid_t lw_gettid(void);