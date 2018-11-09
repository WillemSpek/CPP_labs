/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "simulate.h"


/* Add any global variables you may need. */
typedef struct{
    int start;
    int end;
    double *old_array;
    double *current_array;
    double *next_array;
} update_args;

#define WAVE_PARAM 0.15;


/* Add any functions you may need (like a worker) here. */
void *update_wave (void *p) {
    update_args *args = (update_args *) p;
    int start = args->start;
    int end = args->end;
    double *old_array = args->old_array;
    double *current_array = args->current_array;
    double *next_array = args->next_array;

    for (int i = start; i < end; i ++) {
        next_array[i] = 2 * current_array[i] - old_array[i] + 0.15 *
        (current_array[i - 1] - 2 * current_array[i] - current_array[i +  1]);
    }

    return NULL;
}


/*
 * Executes the entire simulation.
 *
 * Implement your code here.
 *
 * i_max: how many data points are on a single wave
 * t_max: how many iterations the simulation should run
 * num_threads: how many threads to use (excluding the main threads)
 * old_array: array of size i_max filled with data for t-1
 * current_array: array of size i_max filled with data for t
 * next_array: array of size i_max. You should fill this with t+1
 */
double *simulate(const int i_max, const int t_max, const int num_threads,
        double *old_array, double *current_array, double *next_array)
{



    /*
     * After each timestep, you should swap the buffers around. Watch out none
     * of the threads actually use the buffers at that time.
     */

    pthread_t thread_ids[num_threads];

    int iterations_per_thread = (i_max - 2) / num_threads;
    int rem_iterations = (i_max - 2) % num_threads;

    for (int t = 0; t < t_max; t++) {
        // update array per timepoint
        current_array = next_array;
        old_array = current_array;
        // initiliaze first and last wavepoint
        next_array[0] = current_array[0];
        next_array[i_max] = current_array[i_max];


        for (int i = 0; i < num_threads; i++) {
            update_args *args = (update_args*) malloc(sizeof(update_args));
            if (!args) {
                printf("Rip world\n");
            }

            int iterations = iterations_per_thread + rem_iterations / (i + 1);
            int start = iterations * i;
            int end = start + iterations;

            args->start = start;
            args->end = end;
            args->old_array = old_array;
            args->current_array = current_array;
            args->next_array = next_array;

            pthread_create (&thread_ids[i],
                            NULL,
                            &update_wave,
                            (void *)&args);
        }
    }


    /* You should return a pointer to the array with the final results. */
    return current_array;
}
