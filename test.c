#include <stdio.h>
#include <stdlib.h>
#include "lwp.h"
#include "scheduler.h"

int main() {
    printf("starting lwp!\n"); 

    lwp_start();
    printf("creating thread\n")
    lwp_create(NULL, NULL);

    
    return 0;

}