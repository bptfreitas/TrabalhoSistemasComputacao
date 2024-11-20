#ifndef __LIBAUX_H__

#define __LIBAUX_H__

#include <defs.h>

/**
 * Returns the element (line, column) of a matrix pointed by *mat
 */
double matrix_value(int line, int column, int n_cols, double *mat);

/**
 * Prints a matrix
 * 
 */
void print_matrix( double *mat, int n_lines, int n_columns );

/**
 * 
 * Tranlates any newline character to a null byte character ( '\0' )
 */
void strip_newline(char *str);

#endif