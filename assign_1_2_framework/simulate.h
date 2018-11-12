/*
 * simulate.h
 */

#pragma once

double *simulate(const int i_max, const int t_max, const int num_threads, const int block_size,
        double *old_array, double *current_array, double *next_array);
