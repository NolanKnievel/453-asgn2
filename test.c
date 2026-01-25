#include <stdio.h>
#include <stdlib.h>
#include "lwp.h"
#include "scheduler.h"



void print_fun(void *arg) {
    printf("hello from a thread!\n");
    lwp_yield();
    return;
}

int main() {
    printf("starting lwp!\n"); 

    lwp_start();
    printf("creating thread\n");

    lwp_create((lwpfun)print_fun, NULL);
    lwp_create((lwpfun)print_fun, NULL);
    lwp_create((lwpfun)print_fun, NULL);
    printf("threads created\n");

    printf("yielding to next thread\n");
    lwp_yield(); // yield control to next thread in schedule
    lwp_yield();

    return 0;

}
