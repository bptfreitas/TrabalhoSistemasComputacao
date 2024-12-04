#include <cp1.h>
#include <defs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libaux.h>
#include <pthread.h>

pthread_mutex_t cp1_id_lock = PTHREAD_MUTEX_INITIALIZER;
int cp1_id_counter = 0;

pthread_mutex_t cp1_thread_count_lock = PTHREAD_MUTEX_INITIALIZER;
int cp1_thread_count = 0;

void *cp1(void *args) {
    buffer_t *buffers = (buffer_t *)args;
    buffer_t *buffer_do_produtor = &buffers[0];
    buffer_t *buffer_para_cp2 = &buffers[1];

    int error_count = 0, retval;

    char newname[256];
    char processing[]=".processing";

    // Unique thread id
    int cp1_id;

    pthread_mutex_lock( & cp1_id_lock );
    cp1_id = cp1_id_counter;
    cp1_id_counter ++;
    pthread_mutex_unlock( & cp1_id_lock );

    pthread_mutex_lock( & cp1_thread_count_lock );
    cp1_thread_count ++;
    pthread_mutex_unlock( & cp1_thread_count_lock );     

    fprintf(stdout, "\n[CP1 %d] Starting CP1 thread...", cp1_id);

    while (1) {
        if (sem_wait(&buffer_do_produtor->empty) != 0) {
            perror("Failed to lock buffer (empty) in CP1");
            error_count++;
            if (error_count > MAX_ERRORS) {
                fprintf(stderr, "Too many errors in CP1, exiting thread\n");
                pthread_exit(NULL);
            }
            continue;
        }

        if (sem_wait(&buffer_do_produtor->mutex) != 0) {
            perror("Failed to lock buffer (mutex) in CP1");
            error_count++;
            if (error_count > MAX_ERRORS) {
                fprintf(stderr, "Too many errors in CP1, exiting thread\n");
                pthread_exit(NULL);
            }
            continue;
        }

        int out = buffer_do_produtor->out % BUFFER_SIZE;
        S_t *data = buffer_do_produtor->data[out];
        buffer_do_produtor->data[out] = NULL;
        buffer_do_produtor->out++;

        if (sem_post(&buffer_do_produtor->mutex) != 0) {
            perror("Failed to unlock buffer (mutex) in CP1");
            error_count++;
            if (error_count > MAX_ERRORS) {
                fprintf(stderr, "Too many errors in CP1, exiting thread\n");
                pthread_exit(NULL);
            }
            continue;
        }

        if (sem_post(&buffer_do_produtor->full) != 0) {
            perror("Failed to unlock buffer (full) in CP1");
            error_count++;
            if (error_count > MAX_ERRORS) {
                fprintf(stderr, "Too many errors in CP1, exiting thread\n");
                pthread_exit(NULL);
            }
            continue;
        }

        if (data->work_type == WORK_END_THREAD_CONSUMER) {
            pass_work(data, buffer_para_cp2);
            continue;
        }

        if (data->work_type == WORK_END_THREAD_CP3) {
            pass_work(data, buffer_para_cp2);
            continue;
        }

        if (data->work_type == WORK_END_THREAD_CP2) {
            pass_work(data, buffer_para_cp2);
            continue;
        }        

        if (data->work_type == WORK_END_THREAD_CP1) {

            fprintf(stdout, 
                "\n[CP1 %d] Received exit signal",
                cp1_id);

            fflush(stdout);

            pthread_mutex_lock( & cp1_thread_count_lock );
            cp1_thread_count --;
            pthread_mutex_unlock( & cp1_thread_count_lock );                 

            free(data);
            pthread_exit(NULL);
        }

        strcpy(newname, data->source_filename);

        strcpy( &newname[data->extension_pos], processing);

        retval = rename( data->source_filename, newname );

        if (retval == -1){
            perror(" ");            
        } else {
            fprintf(stdout, "\n[CP1 %d] Processing: %s", cp1_id, newname );
        }        

        fprintf(stdout, "\n[CP1 %d] Multiplying matrixes from '%s'...", cp1_id, data->source_filename );

        #pragma omp parallel for num_threads(2)
        for (int i = 0; i < MATRIX_LINES; i++) {
            for (int j = 0; j < MATRIX_COLS; j++) {
                data->C[i][j] = 0.0;
                for (int k = 0; k < MATRIX_COLS; k++) {
                    data->C[i][j] += data->A[i][k] * data->B[k][j];
                }
            }
        }

        print_matrix( data->C, MATRIX_LINES, MATRIX_COLS );
        pass_work(data, buffer_para_cp2);
    }
}
