#include <libaux.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>

void print_matrix( double mat[ MATRIX_LINES][MATRIX_COLS], int n_lines, int n_columns ){

    for (int i = 0; i< n_lines; i++){

        for (int j = 0; j < n_columns; j++ ){
            
            fprintf(stderr, "%.1f ", mat[ i ][ j ] );

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

void pass_work(S_t *work, buffer_t* buf){

    sem_wait( & buf->full );

    sem_wait( & buf->mutex );

    int in = buf->in % BUFFER_SIZE ;

    buf->data[ in ].data = work;

    buf->in++;

    sem_post( & buf->mutex );

    sem_post( & buf->empty );


}


