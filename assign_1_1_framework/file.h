/*
 * file.h
 *
 * Contains several functions for file I/O.
 *
 */

#pragma once

void file_read_double_array(const char *filename, double *array, int n);
void file_write_double_array(const char *filename, double *array, int n);
