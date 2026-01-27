#include <stdio.h>
#include <stdlib.h>
#include "lwp.h"
#include "scheduler.h"



void print_fun(void *arg) {
    printf("hello from a thread!\n");
    lwp_yield();
    return;
}

void wait_fun(void *args) {
    printf("starting to wait\n");
    unsigned int status = -1;
    thread exited_thread = lwp_wait(status);

    printf("thread %d exited with status %d\n", exited_thread->tid, status);


}

int main() {

    lwp_start();

    lwp_create((lwpfun)print_fun, NULL);
    lwp_create((lwpfun)wait_fun, NULL);
    lwp_create((lwpfun)print_fun, NULL);

    lwp_yield(); // yield control to next thread in schedule
    lwp_yield();

    return 0;

}
