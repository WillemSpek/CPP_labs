/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "simulate.h"


/* Struct of arguments for the threads. */
typedef struct{
    int start;
    int end;
} update_args;

// c
#define WAVE_PARAM 0.15

/* global variables */
double *global_old_array;
double *global_current_array;
double *global_next_array;
int global_t_max;
pthread_barrier_t global_barrier;
pthread_mutex_t global_lock;


/* Wave calculation thread.  */
void *update_wave (void *p) {
    update_args *args = (update_args *) p;
    int start = args->start;
    int end = args->end;
    int switched;


    for (int t = 0; t < global_t_max; t++) {
        switched = 0;
        for (int i = start; i < end; i++) {
            global_next_array[i] = 2 * global_current_array[i] - global_old_array[i] + 0.15 *
            (global_current_array[i - 1] - (2 * global_current_array[i] - global_current_array[i +  1]));
        }

        // Only switch the arrays once, since they are shared memory.
        while(!switched) {
            pthread_mutex_trylock(&global_lock);
            switched = 1;
            global_old_array = global_current_array;
            global_current_array = global_next_array;
            global_next_array = global_old_array;
            pthread_mutex_unlock(&global_lock);
        }

        pthread_barrier_wait(&global_barrier);
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
    global_old_array = old_array;
    global_current_array = current_array;
    global_next_array = next_array;
    global_t_max = t_max;

    pthread_barrier_init(&global_barrier, NULL, num_threads);

    int iterations_per_thread = (i_max- 2) / num_threads;
    int rem_iterations = (i_max - 2) % num_threads;

    void *results;
    update_args *args[num_threads];
    int total_iterations = 0;

    for (int i = 0; i < num_threads; i++) {
        int iterations = iterations_per_thread;
        if (i < rem_iterations) {
            iterations++;
        }

        args[i] = (update_args*) malloc(sizeof(update_args));
        // Calculate the start en end of the chunk that can be parallelized.
        int start = total_iterations + 1;
        int end = start + iterations;
        total_iterations += iterations;

        args[i]->start = start;
        args[i]->end = end;

        pthread_create (&thread_ids[i],
                        NULL,
                        &update_wave,
                        (void *)args[i]);
    }

    // Close the threads after use.
    for (int i = 0; i < num_threads; i++) {
        pthread_join(thread_ids[i], &results);
        free(args[i]);
    }


    /* You should return a pointer to the array with the final results. */
    return current_array;
}
