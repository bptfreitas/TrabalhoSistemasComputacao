#include <signal.h>
#include <syslog.h>

#include <cp2.h>
#include <defs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libaux.h>
#include <pthread.h>


pthread_mutex_t cp2_id_lock = PTHREAD_MUTEX_INITIALIZER;
int cp2_id_counter = 0;

pthread_mutex_t cp2_thread_count_lock = PTHREAD_MUTEX_INITIALIZER;
int cp2_thread_count = 0;


void *cp2(void *args) {
    buffer_t *buffers = (buffer_t *)args;
    buffer_t *buffer_do_cp1 = &buffers[1];
    buffer_t *buffer_para_cp3 = &buffers[2];


    int cp2_id;

    pthread_mutex_lock( & cp2_id_lock );
    cp2_id = cp2_id_counter;
    cp2_id_counter ++;
    pthread_mutex_unlock( & cp2_id_lock ); 

    pthread_mutex_lock( & cp2_thread_count_lock );
    cp2_thread_count ++;
    pthread_mutex_unlock( & cp2_thread_count_lock );

    syslog( LOG_INFO, "[CP2 %d] Starting CP2 thread...", cp2_id);    

    int error_count = 0;

    while (1) {
        if (sem_wait(&buffer_do_cp1->empty) != 0) {
            perror("Failed to lock buffer (empty) in CP2");
            error_count++;
            if (error_count > MAX_ERRORS) {
                syslog( LOG_ERR, "[CP2 %d] Too many errors in CP2, exiting thread", cp2_id);
                pthread_exit(NULL);
            }
            continue;
        }
        if (sem_wait(&buffer_do_cp1->mutex) != 0) {
            perror("Failed to lock buffer (mutex) in CP2");
            error_count++;
            if (error_count > MAX_ERRORS) {
                syslog( LOG_ERR, "[CP2 %d] Too many errors in CP2, exiting thread", cp2_id);
                pthread_exit(NULL);
            }
            continue;
        }

        int out = buffer_do_cp1->out % BUFFER_SIZE;
        S_t *data = buffer_do_cp1->data[out];
        buffer_do_cp1->data[out] = NULL;
        buffer_do_cp1->out++;

        if (sem_post(&buffer_do_cp1->mutex) != 0) {
            perror("Failed to unlock buffer (mutex) in CP2");
            error_count++;
            if (error_count > MAX_ERRORS) {
                syslog( LOG_ERR, "[CP2 %d] Too many errors in CP2, exiting thread", cp2_id );
                pthread_exit(NULL);
            }
            continue;
        }

        if (sem_post(&buffer_do_cp1->full) != 0) {
            perror("Failed to unlock buffer (full) in CP2");
            error_count++;
            if (error_count > MAX_ERRORS) {
                syslog( LOG_ERR, "[CP2 %d] Too many errors in CP2, exiting thread", cp2_id);
                pthread_exit(NULL);
            }
            continue;            
        }

        if (data->work_type == WORK_END_THREAD_CONSUMER) {
            pass_work(data, buffer_para_cp3);
            continue;
        }

        if (data->work_type == WORK_END_THREAD_CP3) {
            pass_work(data, buffer_para_cp3);
            continue;
        }

        if (data->work_type == WORK_END_THREAD_CP2) {

            syslog( LOG_INFO, 
                "[CP2 %d] Received exit signal",
                cp2_id);

            fflush(stdout);

            free(data);

            pthread_mutex_lock( & cp2_thread_count_lock );
            cp2_thread_count --;
            pthread_mutex_unlock( & cp2_thread_count_lock );

            pthread_exit(NULL);

        }

        syslog( LOG_INFO, "[CP2 %d] Computing sum of columns of Matrix C from '%s'...", cp2_id, data->source_filename );

		#pragma omp parallel for num_threads(2)
        for (int j = 0; j < MATRIX_COLS; j++) {
            data->V[j] = 0.0;
            for (int i = 0; i < MATRIX_LINES; i++) {
                data->V[j] += data->C[i][j];
            }
        }

        pass_work(data, buffer_para_cp3);
    }
}
