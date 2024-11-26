#include <produtor.h>
#include <libaux.h>

#include <pthread.h>

#include <defs.h>

#include <stdio.h>
#include <string.h>

#include <stdlib.h>

pthread_mutex_t producer_id_lock = PTHREAD_MUTEX_INITIALIZER;
int producer_id_counter = 0;

void *produtor( void* args ){

    buffer_t *buffers = (buffer_t*)args;

    buffer_t *buffer_produtor = &buffers[0];

    int producer_id;

    pthread_mutex_lock( & producer_id_lock );
    producer_id = producer_id_counter;
    producer_id_counter ++;
    pthread_mutex_unlock( & producer_id_lock );


    FILE* entrada_fd = fopen("entrada.in", "r");

    if ( entrada_fd == 0 ){

        perror("ERROR!");
        return NULL;
    }

    char filename_buf[256];

    while ( ! feof( entrada_fd ) ){

        char *filename = fgets( filename_buf, 256, entrada_fd );

        if (filename == NULL ){

            fprintf(stdout, "\nNo more files to process, exiting");            

            break;

        }

        strip_newline( filename_buf );

        fprintf(stdout, 
            "\n[Producer %d] Reading file '%s'...", 
            producer_id,
            filename_buf);

        FILE *matrix_fd = fopen( filename_buf, "r" );

        if ( matrix_fd == NULL ){
            perror("Error!");

            fprintf(stderr, "\nAborting and skipping to next file");

            continue;
        }

        S_t *new_data = (S_t*)malloc( sizeof(S_t) );

        new_data->work_type = WORK_NORMAL;

        strcpy( new_data->source_filename, filename_buf);        

        fprintf(stdout, 
            "\n[Producer %d] Reading Matrix A ...\n",
            producer_id);

        for ( int i =0; i < MATRIX_LINES; i++){

            for ( int j = 0; j < MATRIX_COLS; j++){

                double *ptr = & ( new_data->A[ i ][ j ] );

                fscanf( matrix_fd , "%lf ", ptr );

            }

        }    

        print_matrix( new_data->A, MATRIX_LINES, MATRIX_COLS );

        // Reading matrix B ...
        fprintf(stdout, 
            "\n[Producer %d] Reading Matrix B ...\n",
            producer_id);

        for ( int i =0; i < MATRIX_LINES; i++){

            for ( int j = 0; j < MATRIX_COLS; j++){                

                double *ptr = & ( new_data->B[ i ][ j ] );

                fscanf( matrix_fd , "%lf ", ptr );                

            }

        }

        print_matrix( new_data->B, MATRIX_LINES, MATRIX_COLS );

        fclose( matrix_fd );

        sem_wait( &buffer_produtor->full );

        sem_wait( &buffer_produtor->mutex );

        int in = buffer_produtor->in % BUFFER_SIZE;

        buffer_produtor->data[ in ] = new_data;

        buffer_produtor->in ++;

        sem_post( &buffer_produtor->mutex );

        sem_post( &buffer_produtor->empty );

    }

    fclose( entrada_fd );

#if NOT_YET
    // Sending work types to end all threads

    for (int i = 0; i < N_CONSUMIDORES; i++){

        sem_wait( &buffer_produtor->full );

        S_t* new_data = (S_t*)malloc( sizeof(S_t) );

        new_data->work_type = WORK_END_THREAD_CONSUMER;

        sem_wait( &buffer_produtor->mutex );

        int index = buffer_produtor->in % BUFFER_SIZE;

        buffer_produtor->data[ index ] = new_data;

        buffer_produtor->in ++;

        sem_post( &buffer_produtor->mutex );

        sem_post( &buffer_produtor->empty );
    }

    // CP3
    for (int i = 0; i < N_CP3; i++){

        sem_wait( &buffer_produtor->full );

        S_t* new_data = (S_t*)malloc( sizeof(S_t) );

        new_data->work_type = WORK_END_THREAD_CP3;

        sem_wait( &buffer_produtor->mutex );

        int index = buffer_produtor->in % BUFFER_SIZE;

        buffer_produtor->data[ index ] = new_data;

        buffer_produtor->in ++;

        sem_post( &buffer_produtor->mutex );

        sem_post( &buffer_produtor->empty );
    }

    // CP2
    for (int i = 0; i < N_CP2; i++){

        sem_wait( &buffer_produtor->full );

        S_t* new_data = (S_t*)malloc( sizeof(S_t) );

        new_data->work_type = WORK_END_THREAD_CP2;

        sem_wait( &buffer_produtor->mutex );

        int index = buffer_produtor->in % BUFFER_SIZE;

        buffer_produtor->data[ index ] = new_data;

        buffer_produtor->in ++;

        sem_post( &buffer_produtor->mutex );

        sem_post( &buffer_produtor->empty );
    }        
    
    // Ending CP1 ...
    for (int i = 0; i < N_CP1; i++){

        sem_wait( &buffer_produtor->full );

        S_t* new_data = (S_t*)malloc( sizeof(S_t) );

        new_data->work_type = WORK_END_THREAD_CP1;

        sem_wait( &buffer_produtor->mutex );

        int index = buffer_produtor->in % BUFFER_SIZE;

        buffer_produtor->data[ index ] = new_data;

        buffer_produtor->in ++;

        sem_post( &buffer_produtor->mutex );

        sem_post( &buffer_produtor->empty );

    }

#endif


}