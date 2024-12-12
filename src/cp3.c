#include <signal.h>
#include <syslog.h>

#include <cp3.h>
#include <defs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libaux.h>

#include <pthread.h>

pthread_mutex_t cp3_id_lock = PTHREAD_MUTEX_INITIALIZER;
int cp3_id_counter = 0;

pthread_mutex_t cp3_thread_count_lock = PTHREAD_MUTEX_INITIALIZER;
int cp3_thread_count = 0;

void *cp3(void *args) {

    buffer_t *buffers = (buffer_t *)args;
    buffer_t *buffer_do_cp2 = &buffers[2];
    buffer_t *buffer_para_consumidor = &buffers[3];

    int cp3_id;

    pthread_mutex_lock( & cp3_id_lock );
    cp3_id = cp3_id_counter;
    cp3_id_counter ++;
    pthread_mutex_unlock( & cp3_id_lock ); 

    pthread_mutex_lock( & cp3_thread_count_lock );
    cp3_thread_count ++;
    pthread_mutex_unlock( & cp3_thread_count_lock );

    syslog( LOG_INFO, "[CP3 %d] Starting CP3 thread...", cp3_id);    

    int error_count = 0;

    while (1) {
        if (sem_wait(&buffer_do_cp2->empty) != 0) {
            perror("Failed to lock buffer (empty) in CP3");
            error_count++;
            if (error_count > MAX_ERRORS) {
                syslog( LOG_ERR, "[CP3 %d] Too many errors in CP3, exiting thread", cp3_id);
                pthread_exit(NULL);
            }
            continue;
        }

        if (sem_wait(&buffer_do_cp2->mutex) != 0) {
            perror("Failed to lock buffer (mutex) in CP3");
            error_count++;
            if (error_count > MAX_ERRORS) {
                syslog( LOG_ERR, "[CP3 %d] Too many errors in CP3, exiting thread", cp3_id);
                pthread_exit(NULL);
            }
            continue;
        }

        int out = buffer_do_cp2->out % BUFFER_SIZE;
        S_t *data = buffer_do_cp2->data[out];
        buffer_do_cp2->data[out] = NULL;
        buffer_do_cp2->out++;

        if (sem_post(&buffer_do_cp2->mutex) != 0) {
            perror("Failed to unlock buffer (mutex) in CP3");
            error_count++;
            if (error_count > MAX_ERRORS) {
                syslog( LOG_ERR, "[CP3 %d] Too many errors in CP3, exiting thread", cp3_id);
                pthread_exit(NULL);
            }
            continue;
        }

        if (sem_post(&buffer_do_cp2->full) != 0) {
            perror("Failed to unlock buffer (full) in CP3");
            error_count++;
            if (error_count > MAX_ERRORS) {
                syslog( LOG_ERR, "[CP3 %d] Too many errors in CP3, exiting thread", cp3_id);
                pthread_exit(NULL);
            }
            continue;
        }

        if (data->work_type == WORK_END_THREAD_CONSUMER) {

            pass_work(data, buffer_para_consumidor);
            continue;

        }

        if (data->work_type == WORK_END_THREAD_CP3) {

            syslog( LOG_INFO, "[CP3 %d] Received exit signal", cp3_id);

            free(data);

            pthread_mutex_lock( & cp3_thread_count_lock );
            cp3_thread_count--;
            pthread_mutex_unlock( & cp3_thread_count_lock );            

            pthread_exit(NULL);
        }

        float sum = 0.0;
        for (int j = 0; j < MATRIX_COLS; j++) {
            sum += data->V[j];
        }

        data->E = sum;


        syslog( LOG_INFO, "[CP3 %d] Final sum from '%s': %.3lf", cp3_id, data->source_filename, data->E);

        pass_work(data, buffer_para_consumidor);
    }
}
