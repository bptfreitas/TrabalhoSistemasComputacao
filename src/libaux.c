#include <libaux.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>


void print_matrix( double *mat, int n_lines, int n_columns ){

    for (int i = 0; i< n_lines; i++){

        for (int j = 0; j < n_columns; j++ ){
            
            fprintf(stderr, "%.1f ", mat[i*n_columns + j] );

        }

        fprintf(stderr, "\n");

    }

}

void strip_newline(char *str){

    for (int i = 0; i < strlen(str); i++ ){

        if ( str[i] == '\n'){

            str[i] = '\0';
            
        }
    }
}