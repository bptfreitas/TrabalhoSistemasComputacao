#include <consumidor.h>

#include <stdio.h>

#include <string.h>

#include <stdlib.h>

#include <pthread.h>

#include <defs.h>
#include <cp3.h>

/**
 * This is just for fancy printing
 */
pthread_mutex_t consumer_id_counter_lock = PTHREAD_MUTEX_INITIALIZER;
int consumer_id_counter = 0;

// Mutex to lock access to file to avoid race conditions on writing data
pthread_mutex_t file_lock = PTHREAD_MUTEX_INITIALIZER;


void save_matrix( double mat[MATRIX_LINES][MATRIX_COLS], int lines, int cols, FILE* file_d){    

    for ( int i = 0; i < MATRIX_LINES; i ++){

        fprintf( file_d, "\n");

        for (int j = 0; j < MATRIX_COLS; j++){

            fprintf( file_d, "%.3f ", mat[ i ][ j ]);

        }
    }
}

void save_vector( double vector[MATRIX_COLS], int size, FILE* file_d){

    fprintf( file_d, "\n");

    for (int i=0; i < size; i++){

        fprintf( file_d, "%.3f ", vector[ i ]);

    }
}

void* consumidor(void* args){

    buffer_t *buffers = (buffer_t*)args;

    buffer_t *buffer_do_cp3 = &buffers[3];

    pthread_mutex_lock(&consumer_id_counter_lock);
    int consumer_id = consumer_id_counter;

    fprintf(stdout, "\n[Consumer %d] Starting consumer thread ...", consumer_id );

    if (consumer_id == 0){
        fprintf(stdout, "\n[Consumer %d] Truncating 'saida.out' ... ", consumer_id );

        FILE *fake_ptr = fopen("saida.out", "w");

        fclose( fake_ptr );        
    }

    consumer_id_counter++;
    pthread_mutex_unlock(&consumer_id_counter_lock);

    while (1){

        sem_wait( &buffer_do_cp3->empty );

        S_t *data;

        int out;

        sem_wait( &buffer_do_cp3->mutex );    

        out = buffer_do_cp3->out;

        data = buffer_do_cp3->data[ out % BUFFER_SIZE ];

        buffer_do_cp3->data[ out % BUFFER_SIZE ] = NULL;

        buffer_do_cp3->out ++;

        sem_post( &buffer_do_cp3->mutex );

        sem_post( &buffer_do_cp3->full );

        if ( data->work_type == WORK_END_THREAD_CONSUMER ){

            fprintf(stdout, 
                "\n[Consumer %d] Received exit signal",
                consumer_id);

            fflush(stdout);

            free(data);

            pthread_exit( NULL );
        }

        pthread_mutex_lock( &file_lock );

        FILE *output_fd = fopen( "saida.out", "a");

        fprintf(stdout, "\n[Consumer %d] Saving results of '%s' ... ", 
            consumer_id,
            data->source_filename);

        if (output_fd == NULL){

            perror("Error:");

            continue;

        }
        
        // Saving output ...
        fprintf( output_fd, "\n================================");

        fprintf( output_fd, "\nEntrada: %s", data->source_filename );

        fprintf( output_fd, "\nA:");

        save_matrix( data->A, MATRIX_LINES, MATRIX_COLS, output_fd);

        fprintf( output_fd, "\nB:");

        save_matrix( data->B, MATRIX_LINES, MATRIX_COLS, output_fd);

        fprintf( output_fd, "\nC:");

        save_matrix( data->C, MATRIX_LINES, MATRIX_COLS, output_fd);

        fprintf( output_fd, "\nV:");

        save_vector( data->V, MATRIX_LINES, output_fd);

        fprintf( output_fd, "\nE:\n%.3f", data->E );

        fprintf( output_fd, "\n================================");        

        fclose( output_fd );
    
        pthread_mutex_unlock( &file_lock );
    
        // Cleaning memory ...         
        free( data );

    }
    
}
