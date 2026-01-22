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

    // case - queue length 1 and removing head - empty the queue
    if (queue_length == 1 && victim == ready_head) {
        ready_head = NULL;
        return;
    }

    // case - removing head
    if (victim == ready_head) {
        ready_head = ready_head->next; // advance head 
    }


    // traverse to victim, if we reach head again do nothing(victim not here)
    thread current_thread = ready_head->next;
    while (current_thread != ready_head) {
        current_thread = current_thread->next;
    }

    // at front and didn't find victim
    if(current_thread == ready_head && victim != ready_head) {
        // got back to front - victim DNE in queue
        // error - victim not found
        fprintf(stderr, "Error: Thread not found in scheduler\n");
        return;
    }
    
    // remove victim
    thread front_thread = current_thread->next;
    thread back_thread = current_thread->prev;
    front_thread->prev = back_thread;
    back_thread->next = front_thread;
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


