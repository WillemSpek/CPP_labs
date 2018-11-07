#include <stdio.h>
#include <math.h>

#include "generatedata.h"

/*
 * Simple gauss with mu=0, sigma^1=1
 */
double gauss(double x)
{
    return exp((-1 * x * x) / 2);
}

/*
 * Fills a given array with samples of a given function. The first sample is
 * placed at array index `offset'. `range' samples are taken, so your array
 * should be able to store at least offset+range doubles. The function `f' is
 * sampled `range' times between `sample_start' and `sample_end'.
 */
void fill(double *array, int offset, int range, double sample_start,
        double sample_end, func_t f)
{
    int i;
    float dx;

    dx = (sample_end - sample_start) / range;
    for (i = 0; i < range; i++) {
        array[i + offset] = f(sample_start + i * dx);
        printf("%d: %f %f\n", i+offset, sample_start+i*dx, f(sample_start+i*dx));
    }
}

