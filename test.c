#include <stdio.h>
#include <stdlib.h>
#include "lwp.h"
#include "scheduler.h"



void print_fun(void *arg) {
    printf("hello from a thread!\n");
    lwp_exit(5);
    return;
}

void print_fun2(void *arg) {
    printf("hello from a thread 2!\n");
    lwp_yield();
    lwp_exit(5);
    return;
}


void wait_fun(void *args) {
    printf("starting to wait\n");
    int status = -1;
    unsigned long exited_thread = lwp_wait(&status);
    printf("thread %lu exited with status %d\n", exited_thread, LWPTERMSTAT(status));
    lwp_exit(4);
}

int main() {

    lwp_start();
    lwp_create((lwpfun)wait_fun, NULL);
    lwp_create((lwpfun)print_fun, NULL);
    lwp_create((lwpfun)wait_fun, NULL);
    lwp_create((lwpfun)print_fun2, NULL);
    lwp_create((lwpfun)wait_fun, NULL);


    lwp_yield(); // yield control to next thread in schedule
    lwp_yield();

    return 0;

}
