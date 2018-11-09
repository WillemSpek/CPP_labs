/*
 * file.c
 *
 * Contains several functions for file I/O.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/*
 * Reads at most n doubles from a given file into an array.
 */
void file_read_double_array(const char *filename, double *array, int n)
{
    FILE *fp;
    int i;

    fp = fopen(filename, "r");

    if (!fp) {
        fprintf(stderr, "Failed to open file %s: %s\n", filename,
                strerror(errno));
        exit(-1);
    }

    for (i = 0; i < n; i++) {
        if (feof(fp))
            break;
        fscanf(fp, "%lf", &array[i]);
    }

    fclose(fp);
}

/*
 * Saves an array with n items to a given file, overwriting any previous
 * contents.
 */
void file_write_double_array(const char *filename, double *array, int n)
{
    FILE *fp;
    int i;

    fp = fopen(filename, "w");

    if (!fp) {
        fprintf(stderr, "Failed to open file %s: %s\n", filename,
                strerror(errno));
        exit(-1);
    }

    for (i = 0; i < n; i++) {
        fprintf(fp, "%f\n", array[i]);
    }

    fclose(fp);
}



