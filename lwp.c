#define next sched_one
#define prev sched_two

#include <stdlib.h>
#include <stdio.h>
#include "lwp.h"
#include "fp.h"

// RULES
// saved bp/end of args must be divisible by 16




// ---- SCHEDULER ----

// global pointer to head of circular ready queue
static thread ready_head = NULL;

// Add the passed context to the scheduler’s scheduling pool
void rr_admit(thread new) {
    // Case - no head
    if(!ready_head) {
        ready_head = new;
        new->next = ready_head;
        new->prev = ready_head;
        return;
    }

    // otherwise, add to back of queue
    thread former_last_thread = ready_head->prev; // save last head of queue
    former_last_thread->next = new;
    ready_head->prev = new;
    new->next = ready_head;
    new->prev = former_last_thread;
    return;
}

// Return the number of runnable threads
int rr_qlen() {
    if (!ready_head) {
        return 0;
    }    

    // traverse LL until we reach ready_head again, keeping count
    int count = 1;
    thread current_thread = ready_head->next;
    while(current_thread != ready_head) {
        current_thread = current_thread->next;
        count++;
    }

    return count;
}


// Remove the passed context from the scheduler’s scheduling pool
void rr_remove(thread victim) {

    if (!ready_head || !victim) return;

    // only one thread in queue
    if (ready_head == victim && ready_head->next == ready_head) {
        ready_head = NULL;
        return;
    }

    // adjust head if victim is head
    if (victim == ready_head) {
        ready_head = ready_head->next;
    }

    thread prev_thread = victim->prev; // prev pointer
    thread next_thread = victim->next; // next pointer

    if (!prev_thread || !next_thread) {
        fprintf(stderr, "Error: Thread has invalid neighbors\n");
        return;
    }

    // skip victim
    prev_thread->next = next_thread;
    next_thread->prev = prev_thread;
    return;

}


// Return the next thread to be run or NULL if there isn’t one
thread rr_next() {
    // if no head, then return NULL
    if(!ready_head) {
        return NULL;
    }
    
    // move head to next, if there is a next available
    thread t = ready_head;
    ready_head = t->next; // advance head

    return t;
}


// NOTE - include rr_next as 5th arg
struct scheduler rr_publish = {NULL, NULL, rr_admit, rr_remove, rr_next, rr_qlen};
scheduler RoundRobin = &rr_publish;


