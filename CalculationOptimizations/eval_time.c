#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "eval_time.h"

static struct rusage rp;
static struct rusage rk;


void init_time()
{
    getrusage(RUSAGE_SELF, &rp);
}


double read_time()
{ 
    getrusage(RUSAGE_SELF, &rk);
    return (rk.ru_utime.tv_usec - rp.ru_utime.tv_usec) * 1.0e-6 
        + rk.ru_utime.tv_sec - rp.ru_utime.tv_sec;
}
