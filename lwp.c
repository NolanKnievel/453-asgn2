#define next sched_one
#define prev sched_two

#include <stdlib.h>
#include <stdio.h>
#include "lwp.h"
#include "fp.h"

// RULES
// saved bp/end of args must be divisible by 16
