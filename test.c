#include <stdio.h>
#include <stdlib.h>
#include "lwp.h"
#include "scheduler.h"

int main() {
    printf("starting lwp!\n"); 

    lwp_start();
    printf("creating thread\n");

    lwp_create(NULL, NULL);
    lwp_create(NULL, NULL);
    lwp_create(NULL, NULL);
    lwp_yield(); // yield control to next thread in schedule
    
    return 0;

}

void print_fun() {
    printf("hello from a thread!");
    return;
}