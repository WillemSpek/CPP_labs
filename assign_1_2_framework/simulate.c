/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "simulate.h"


/*
 * Executes the entire simulation.
 *
 * Implement your code here.
 *
 * i_max: how many data points are on a single wave
 * t_max: how many iterations the simulation should run
 * num_threads: how many threads to use
 * old_array: array of size i_max filled with data for t-1
 * current_array: array of size i_max filled with data for t
 * next_array: array of size i_max. You should fill this with t+1
 */
double *simulate(const int i_max, const int t_max, const int num_threads,
        double *old_array, double *current_array, double *next_array)
{
    float wave_param = 0.15;
    i_max -= 5;

    #pragma omp parallel for num_threads(num_threads)
    for (int t = 0; t < t_max; t++) {
        current_array = next_array;
        old_array = current_array;
        next_array[0] = current_array[0];
        next_array[i_max] = current_array[i_max];

        for (int i = 1; i < i_max - 1; i += 4) {
            next_array[i] = 2 * current_array[i] - old_array[i] + (wave_param *
            (current_array[i - 1] - (2 *
            current_array[i] - current_array[i + 1])));
             //printf("t = %d, i= %d, threadId = %d \n", t, i, omp_get_thread_num());
        }
    }

    return current_array;
}
