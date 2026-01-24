#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "lwp.h"

// ---- Round Robin Schedule Functions ----
void rr_admit(thread new);
void rr_remove(thread victim);
thread rr_next(void);
int rr_qlen(void);
extern void lwp_set_scheduler(scheduler sched);
extern scheduler lwp_get_scheduler(void);

// pre-defined RR scheduler
extern struct scheduler rr_publish;
extern scheduler RoundRobin;

#endif
