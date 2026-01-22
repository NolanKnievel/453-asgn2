#define next sched_one;
#define prev sched_two;
#include "~/pn-cs453/Given/Asgn2/include/lwp.h"
#include "~/pn-cs453/Given/Asgn2/include/fp.h"

// RULES
// saved bp/end of args must be divisible by 16




// ---- SCHEDULER ----

// Add the passed context to the scheduler’s scheduling pool
void rr_admit(thread new) {return;}

// Remove the passed context from the scheduler’s scheduling pool
void rr_remove(thread victim) {return;}


// Return the next thread to be run or NULL if there isn’t one
thread rr_next() {return;}

// Return the number of runnable threads
int rr_qlen() {return;}

struct scheduler rr_publish = {NULL, NULL, rr_admit, rr_remove, rr_next, rr_qlen};
scheduler = &rr_publish;


