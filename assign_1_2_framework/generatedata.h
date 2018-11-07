#pragma once

typedef double (*func_t)(double x);

double gauss(double x);
void fill(double *array, int offset, int range, double sample_start,
        double sample_end, func_t f);
