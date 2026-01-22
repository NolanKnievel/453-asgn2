#define next sched_one
#define prev sched_two

#include <stdlib.h>
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
    thread former_last_thread = ready_head->prev;
    former_last_thread->next = new;
    thread current_head->prev = new;
    new->next = current_head;
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

    int queue_length = rr_qlen();
    // case - queue length 1 - empty the queue
    if (queue_length == 1) {
        ready_head = NULL;
    }

    // traverse to victim, if we reach head again do nothing(victim not here)
    thread current_thread = ready_head;
    thread next_thread = current_thread->next;
    while (next_thread != ready_head) {

        // if next thread is victim, remove it
        if(next_thread == victim) {

            thread new_next_thread = current_thread->next->next;
            current_thread->next = new_next_thread;
            new_next_thread->prev = current_thread;
            return;
        }

        // otherwise keep looking
        current_thread = next_thread;
        next_thread = next_thread->next;

    }

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
    ready_head = t->next;

    return ready_head;
}


// NOTE - include rr_next as 5th arg
struct scheduler rr_publish = {NULL, NULL, rr_admit, rr_remove, rr_next, rr_qlen};
scheduler RoundRobin = &rr_publish;


