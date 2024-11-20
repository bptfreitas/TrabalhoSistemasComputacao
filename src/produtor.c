#include <produtor.h>

#include <defs.h>

#include <stdio.h>
#include <string.h>

#include <stdlib.h>

void print_matrix( double *mat ){

    for (int i = 0; i< MATRIX_LINES; i++){

        for (int j = 0; j < MATRIX_COLS; j++ ){
            
            fprintf(stderr, "%.1f ", mat[i*MATRIX_COLS + j] );

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

void *produtor( void* args ){

    buffer_t *buffers = (buffer_t*)args;

    buffer_t *buffer_produtor = &buffers[0];


    FILE* entrada_fd = fopen("entrada.in", "r");

    if ( entrada_fd == 0 ){

        perror("ERROR!");
        return NULL;
    }

    char filename_buf[256];

    while ( ! feof( entrada_fd ) ){

        char *filename = fgets( filename_buf, 256, entrada_fd );

        if (filename == NULL ){

            break;

        }

        strip_newline( filename_buf );

        fprintf(stdout, "\nReading file '%s'...", filename_buf);

        FILE *matrix_fd = fopen( filename_buf, "r" );

        if ( matrix_fd == NULL ){
            perror("Error!");

            fprintf(stderr, "\nAborting and skipping to next file");

            continue;
        }

        S_t *new_data = (S_t*)malloc( sizeof(S_t) );

        new_data->A = (double*)malloc(sizeof(double)*MATRIX_LINES*MATRIX_COLS);
        new_data->B = (double*)malloc(sizeof(double)*MATRIX_LINES*MATRIX_COLS);
        new_data->C = (double*)malloc(sizeof(double)*MATRIX_LINES*MATRIX_COLS);

        new_data->V = (double*)malloc(sizeof(double)*MATRIX_LINES);

        // Reading matrix A ...
        for ( int i =0; i < MATRIX_LINES; i++){

            for ( int j = 0; j < MATRIX_COLS; j++){

                double *ptr = new_data->A + (i*MATRIX_COLS + j);

                fscanf( matrix_fd , "%lf ", ptr );

            }

        }

        print_matrix( new_data->A );

        // Reading matrix B ...
        for ( int i =0; i < MATRIX_LINES; i++){

            for ( int j = 0; j < MATRIX_COLS; j++){

                double *ptr = new_data->B + (i*MATRIX_COLS + j);

                fscanf( matrix_fd , "%lf ", ptr );

            }

        }

        print_matrix( new_data->B );

        sem_wait( &buffer_produtor->empty );

        sem_wait( &buffer_produtor->mutex );

            int index = buffer_produtor->in % BUFFER_SIZE;

            buffer_produtor->data[ index ] = new_data;

            buffer_produtor->in ++;

        sem_post( &buffer_produtor->mutex );

        sem_post( &buffer_produtor->full );

    }




}