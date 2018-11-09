#include <time.h>

#include "timer.h"

struct timespec start_time;

/*
 * Starts the timing. Get a result by calling timer_end afterwards.
 */
void timer_start(void)
{
    clock_gettime(CLOCK_REALTIME, &start_time);
}

/*
 * Returns the elapsed time since calling timer_start in seconds.
 * Results when calling this without calling timer_end are undefined.
 */
double timer_end(void)
{
    struct timespec end_time;
    clock_gettime(CLOCK_REALTIME, &end_time);

    return difftime(end_time.tv_sec, start_time.tv_sec) +
        (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.;
}


