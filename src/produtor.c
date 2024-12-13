#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <fcntl.h>

#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>

#include <linux/sched.h>

#include <produtor.h>
#include <libaux.h>

#include <defs.h>


pthread_mutex_t producer_id_lock = PTHREAD_MUTEX_INITIALIZER;
int producer_id_counter = 0;

pthread_mutex_t job_counter_lock = PTHREAD_MUTEX_INITIALIZER;
int job_counter = 0;

pthread_mutex_t sigterm_recv_lock = PTHREAD_MUTEX_INITIALIZER;
int sigterm_recv = 0;

extern pthread_mutex_t consumer_thread_count_lock;
extern int consumer_thread_count;

extern pthread_mutex_t cp3_thread_count_lock;
extern int cp3_thread_count;

extern pthread_mutex_t cp2_thread_count_lock;
extern int cp2_thread_count;

extern pthread_mutex_t cp1_thread_count_lock;
extern int cp1_thread_count;

extern pthread_barrier_t stop_controller_barrier;
extern pthread_barrier_t end_barrier;


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

        pthread_mutex_lock( & sigterm_recv_lock );
        if ( sigterm_recv == 1 ){
            // SIGTERM Received, break loop
            syslog(LOG_INFO, "[Produtor %d] SIGTERM received: stopping input processing", producer_id );
            pthread_mutex_unlock( & sigterm_recv_lock );
            break;
        } else {
            pthread_mutex_unlock( & sigterm_recv_lock );
        }

        char *filename = fgets( filename_buf, 256, entrada_fd );

        if (filename == NULL ){

            syslog( LOG_INFO, "[Produtor %d] No more files to process, exiting", producer_id);

            break;

        }

        strip_newline( filename_buf );

        syslog( LOG_INFO, 
            "[Producer %d] Reading file '%s'...", 
            producer_id,
            filename_buf);

        FILE *matrix_fd = fopen( filename_buf, "r" );

        if ( matrix_fd == NULL ){
            perror("Error!");

            syslog( LOG_INFO, "[Producer %d] Aborting and skipping to next file", producer_id);

            continue;
        }

        S_t *new_data = (S_t*)malloc( sizeof(S_t) );

        new_data->work_type = WORK_NORMAL;

        strcpy( new_data->source_filename, filename_buf);   

        syslog( LOG_INFO, 
            "[Producer %d] Reading Matrix A ...",
            producer_id);

        for ( int i =0; i < MATRIX_LINES; i++){

            for ( int j = 0; j < MATRIX_COLS; j++){

                double *ptr = & ( new_data->A[ i ][ j ] );

                fscanf( matrix_fd , "%lf ", ptr );

            }

        }    

        print_matrix( new_data->A, MATRIX_LINES, MATRIX_COLS );

        // Reading matrix B ...
        syslog( LOG_INFO, 
            "[Producer %d] Reading Matrix B ...",
            producer_id);

        for ( int i =0; i < MATRIX_LINES; i++){

            for ( int j = 0; j < MATRIX_COLS; j++){                

                double *ptr = & ( new_data->B[ i ][ j ] );

                fscanf( matrix_fd , "%lf ", ptr );                

            }

        }

        print_matrix( new_data->B, MATRIX_LINES, MATRIX_COLS );

        fclose( matrix_fd );

        pthread_mutex_lock( & job_counter_lock );
        job_counter ++;
        pthread_mutex_unlock( & job_counter_lock );     

        sem_wait( &buffer_produtor->full );

        sem_wait( &buffer_produtor->mutex );

        int in = buffer_produtor->in % BUFFER_SIZE;

        buffer_produtor->data[ in ] = new_data;

        buffer_produtor->in ++;

        sem_post( &buffer_produtor->mutex );

        sem_post( &buffer_produtor->empty );

    }

    fclose( entrada_fd );

    // Stopping thread controller ...
    int controller_fd = open("/tmp/matrix_deamon", O_WRONLY );

    if (controller_fd > 0 ){

        char msg[] = "stop\n";

        write( controller_fd , msg, strlen( msg ) + 1 );

        close( controller_fd );

        pthread_barrier_wait( &stop_controller_barrier );
    } else {

        syslog( LOG_WARNING, 
            "[Producer %d] Couldn't send 'stop' command to thread controller!",
            producer_id);

    }

    // Waiting current jobs to finish ...
    while (1){
        pthread_mutex_lock( & job_counter_lock );
        if (job_counter > 0){
            pthread_mutex_unlock( & job_counter_lock );
            sched_yield();
            continue;
        }
        pthread_mutex_unlock( & job_counter_lock );
        break;
    }

    pthread_mutex_lock( &consumer_thread_count_lock );
    int nthreads = consumer_thread_count;
    pthread_mutex_unlock( &consumer_thread_count_lock );
    // Sending work types to end the consumer
    for (int i = 0; i < nthreads; i++){

        S_t* new_data = (S_t*)malloc( sizeof(S_t) );

        new_data->work_type = WORK_END_THREAD_CONSUMER;

        pass_work( new_data , buffer_produtor );

    }

    // Waiting current jobs to finish ...
    while (1){
        pthread_mutex_lock( & consumer_thread_count_lock );
        if (consumer_thread_count > 0){
            pthread_mutex_unlock( & consumer_thread_count_lock );
            sched_yield();
            continue;
        }
        pthread_mutex_unlock( & consumer_thread_count_lock );
        break;
    }    


    pthread_mutex_lock( &cp3_thread_count_lock );
    nthreads = cp3_thread_count;
    pthread_mutex_unlock( &cp3_thread_count_lock );
    // Sending works to end CP3
    for (int i = 0; i < nthreads; i++){

        S_t* new_data = (S_t*)malloc( sizeof(S_t) );

        new_data->work_type = WORK_END_THREAD_CP3;

        pass_work( new_data , buffer_produtor );
        
    }

    while (1){
        pthread_mutex_lock( & cp3_thread_count_lock );
        if (cp3_thread_count > 0){
            pthread_mutex_unlock( & cp3_thread_count_lock );
            sched_yield();
            continue;
        }
        pthread_mutex_unlock( & cp3_thread_count_lock );
        break;
    }


    // Sending works to end CP2
    pthread_mutex_lock( &cp2_thread_count_lock );
    nthreads = cp2_thread_count;
    pthread_mutex_unlock( &cp2_thread_count_lock );  
    for (int i = 0; i < nthreads; i++){

        S_t* new_data = (S_t*)malloc( sizeof(S_t) );

        new_data->work_type = WORK_END_THREAD_CP2;

        pass_work( new_data , buffer_produtor );
        
    } 
    
    while (1){
        pthread_mutex_lock( & cp2_thread_count_lock );
        if (cp2_thread_count > 0){
            pthread_mutex_unlock( & cp2_thread_count_lock );
            sched_yield();
            continue;
        }
        pthread_mutex_unlock( & cp2_thread_count_lock );
        break;
    }  

    // Sending works to end CP1
    pthread_mutex_lock( &cp1_thread_count_lock );
    nthreads = cp1_thread_count;
    pthread_mutex_unlock( &cp1_thread_count_lock );    
    for (int i = 0; i < nthreads; i++){

        S_t* new_data = (S_t*)malloc( sizeof(S_t) );

        new_data->work_type = WORK_END_THREAD_CP1;

        pass_work( new_data , buffer_produtor );
        
    } 

    while (1){
        pthread_mutex_lock( & cp1_thread_count_lock );
        if (cp1_thread_count > 0){
            pthread_mutex_unlock( & cp1_thread_count_lock );
            sched_yield();
            continue;
        }
        pthread_mutex_unlock( & cp1_thread_count_lock );
        break;
    }

    pthread_barrier_wait( &end_barrier );


}