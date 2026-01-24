#define next sched_one
#define prev sched_two

#include <stdlib.h>
#include <stdio.h>
#include "lwp.h"
#include "fp.h"
#include "scheduler.h"

// RULES
// saved bp/end of args must be divisible by 16
// TODO - move scheduler headers to otehr file



// Starts the threading system. Converts original system thread into a LWP
void lwp_start(void) {


    // create context for calling thread
    struct threadinfo_st t;
    t.tid = 0;
    t.next = &t;
    t.prev = &t;
    t.stack = NULL;
    t.stacksize = 0;
    t.state = NULL;
    t.status = LWP_LIVE;
    t.exited = NULL;
    t.lib_one = NULL;
    t.lib_two = NULL;

    
    // save current register state
    swap_rfiles(&t.state, NULL);
    
    // admit calling thread to scheduler
    lwp_set_scheduler(NULL);
    scheduler s = lwp_get_scheduler();
    s->admit(t);

    // yield to next thread in schedule
    lwp_yield();

}


tid_t lwp_create(lwpfun function, void *argument);
void lwp_yield(void);
void lwp_exit(int exitval);
tid_t lwp_wait(int *status);
tid_t lw_gettid(void);