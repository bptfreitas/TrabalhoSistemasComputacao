#include <cp1.h>
#include <defs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void *cp1(void *args) {
    buffer_t *buffers = (buffer_t *)args;
    buffer_t *buffer_do_produtor = &buffers[0];
    buffer_t *buffer_para_cp2 = &buffers[1];

    int error_count = 0;

    fprintf(stdout, "\nStarting CP1 thread...");

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
        if (data->work_type == WORK_END_THREAD_CP1) {
            free(data);
            pthread_exit(NULL);
        }
        print_matrix( data->A, MATRIX_LINES, MATRIX_COLS );
        print_matrix( data->B, MATRIX_LINES, MATRIX_COLS );
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
