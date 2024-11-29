#include <cp2.h>
#include <defs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void *cp2(void *args) {
    buffer_t *buffers = (buffer_t *)args;
    buffer_t *buffer_do_cp1 = &buffers[1];
    buffer_t *buffer_para_cp3 = &buffers[2];

    int error_count = 0;
    fprintf(stdout, "\nStarting CP2 thread...");

    while (1) {
        if (sem_wait(&buffer_do_cp1->empty) != 0) {
            perror("Failed to lock buffer (empty) in CP2");
            error_count++;
            if (error_count > MAX_ERRORS) {
                fprintf(stderr, "Too many errors in CP2, exiting thread\n");
                pthread_exit(NULL);
            }
            continue;
        }
        if (sem_wait(&buffer_do_cp1->mutex) != 0) {
            perror("Failed to lock buffer (mutex) in CP2");
            error_count++;
            if (error_count > MAX_ERRORS) {
                fprintf(stderr, "Too many errors in CP2, exiting thread\n");
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
                fprintf(stderr, "Too many errors in CP2, exiting thread\n");
                pthread_exit(NULL);
            }
            continue;
        }
        if (sem_post(&buffer_do_cp1->full) != 0) {
            perror("Failed to unlock buffer (full) in CP2");
            error_count++;
            if (error_count > MAX_ERRORS) {
                fprintf(stderr, "Too many errors in CP2, exiting thread\n");
                pthread_exit(NULL);
            }
            continue;
        }

        if (data->work_type == WORK_END_THREAD_CP2) {
            free(data);
            pthread_exit(NULL);
        }
        for (int j = 0; j < MATRIX_COLS; j++) {
            data->V[j] = 0.0;
            for (int i = 0; i < MATRIX_LINES; i++) {
                data->V[j] += data->C[i][j];
            }
        }

        pass_work(data, buffer_para_cp3);
    }
}
