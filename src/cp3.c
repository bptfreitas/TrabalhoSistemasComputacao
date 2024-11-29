#include <cp3.h>
#include <defs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void *cp3(void *args) {
    buffer_t *buffers = (buffer_t *)args;
    buffer_t *buffer_do_cp2 = &buffers[2];
    buffer_t *buffer_para_consumidor = &buffers[3];

    int error_count = 0;

    fprintf(stdout, "\nStarting CP3 thread...");

    while (1) {
        if (sem_wait(&buffer_do_cp2->empty) != 0) {
            perror("Failed to lock buffer (empty) in CP3");
            error_count++;
            if (error_count > MAX_ERRORS) {
                fprintf(stderr, "Too many errors in CP3, exiting thread\n");
                pthread_exit(NULL);
            }
            continue;
        }
        if (sem_wait(&buffer_do_cp2->mutex) != 0) {
            perror("Failed to lock buffer (mutex) in CP3");
            error_count++;
            if (error_count > MAX_ERRORS) {
                fprintf(stderr, "Too many errors in CP3, exiting thread\n");
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
                fprintf(stderr, "Too many errors in CP3, exiting thread\n");
                pthread_exit(NULL);
            }
            continue;
        }

        if (sem_post(&buffer_do_cp2->full) != 0) {
            perror("Failed to unlock buffer (full) in CP3");
            error_count++;
            if (error_count > MAX_ERRORS) {
                fprintf(stderr, "Too many errors in CP3, exiting thread\n");
                pthread_exit(NULL);
            }
            continue;
        }
        if (data->work_type == WORK_END_THREAD_CP3) {
            free(data);
            pthread_exit(NULL);
        }

        float sum = 0.0;
        for (int j = 0; j < MATRIX_COLS; j++) {
            sum += data->V[j];
        }

        printf("Final Sum: %f\n", sum);

        pass_work(data, buffer_para_consumidor);
    }
}
