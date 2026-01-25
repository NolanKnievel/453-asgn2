#include <stdio.h>
#include <stdlib.h>
#include "lwp.h"
#include "scheduler.h"



void print_fun(void *arg) {
    printf("hello from a thread!");
    lwp_yield();
    return;
}

int main() {
    printf("starting lwp!\n"); 

    lwp_start();
    printf("creating thread\n");

    lwp_create((lwpfun)print_fun, NULL);
    printf("thread created\n");

    // lwp_create(print_fun, NULL);
    // lwp_create(print_fun, NULL);
    printf("yielding to next thread");
    lwp_yield(); // yield control to next thread in schedule
    
    return 0;

}
