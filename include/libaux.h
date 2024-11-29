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
void print_matrix( double mat[ MATRIX_LINES][MATRIX_COLS], int n_lines, int n_columns );

/**
 * 
 * Tranlates any newline character to a null byte character ( '\0' )
 */
void strip_newline(char *str);


/**
 * \brief Passs work to a specified buffer 
 * 
 * This procedure is meant to be used when any kind of work different
 * from WORK_NORMAL is received by the current thread
 * 
 * \param work A pointer to the work
 * \param buffer A pointer to the buffer to pass wotrk to
 * 
 * */
void pass_work( S_t* work, buffer_t *buffer);

#endif