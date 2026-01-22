#include <stdio.h>
#include <stdlib.h>
#include "lwp.h"

void rr_admit(thread new);
void rr_remove(thread victim);
thread rr_next(void);
int rr_qlen(void);

// create a dummy thread with a given tid
thread create_dummy_thread(tid_t tid) {
    thread t = malloc(sizeof(struct threadinfo_st));
    t->tid = tid;
    t->sched_one = NULL;
    t->sched_two = NULL;
    return t;
}

// print the circular queue
void print_queue(thread head) {
    if(!head) {
        printf("Queue empty\n");
        return;
    }

    thread current = head;
    do {
        printf("%lu -> ", current->tid);
        current = current->sched_one; // next
    } while(current != head);
    printf("(back to head)\n");
}

int main(void) {
    printf("=== Scheduler Test ===\n");

    // Create dummy threads
    thread t1 = create_dummy_thread(1);
    thread t2 = create_dummy_thread(2);
    thread t3 = create_dummy_thread(3);

    printf("\nAdmit threads...\n");
    rr_admit(t1);
    rr_admit(t2);
    rr_admit(t3);

    printf("Queue length: %d\n", rr_qlen());
    print_queue(t1);

    printf("\nCall rr_next() a few times:\n");
    for(int i = 0; i < 5; i++) {
        thread next = rr_next();
        printf("Next thread: %lu\n", next->tid);
    }

    printf("\nRemove thread 2:\n");
    rr_remove(t2);
    printf("Queue length: %d\n", rr_qlen());
    print_queue(t1);

    printf("\nRemove head thread 1:\n");
    rr_remove(t1);
    printf("Queue length: %d\n", rr_qlen());
    print_queue(t3);

    printf("\nRemove last thread 3:\n");
    rr_remove(t3);
    printf("Queue length: %d\n", rr_qlen());
    print_queue(NULL);

    // Free allocated memory
    free(t1);
    free(t2);
    free(t3);

    return 0;
}
