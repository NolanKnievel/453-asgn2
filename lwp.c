#define next sched_one
#define prev sched_two

#include <stdlib.h>
#include "lwp.h"
#include "fp.h"

// RULES
// saved bp/end of args must be divisible by 16




// ---- SCHEDULER ----

// Add the passed context to the scheduler’s scheduling pool
void rr_admit(thread new) {}

// Remove the passed context from the scheduler’s scheduling pool
void rr_remove(thread victim) {}


// Return the next thread to be run or NULL if there isn’t one

// thread rr_next() {return; }

// Return the number of runnable threads
int rr_qlen() {return 0;}

// NOTE - include rr_next as 5th arg
struct scheduler rr_publish = {NULL, NULL, rr_admit, rr_remove, NULL, rr_qlen};
scheduler RoundRobin = &rr_publish;


