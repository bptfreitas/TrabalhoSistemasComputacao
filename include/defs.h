#ifndef __DEFS_H___

#define __DEFS_H___

#include <semaphore.h>

#define MATRIX_LINES 2
#define MATRIX_COLS 2


#define N_PRODUTORES 1
#define N_CP1 1
#define N_CP2 1
#define N_CP3 1
#define N_CONSUMIDORES 1

#define TOTAL_THREADS ( N_PRODUTORES + N_CP1 + N_CP2 + N_CP3 + N_CONSUMIDORES)

#define BUFFER_SIZE 5

typedef struct{

    char source_filename[256];

    double *A, *B, *C;
    
    double *V;

    double E;    

} S_t;

typedef struct {

    S_t data[ BUFFER_SIZE ];

    sem_t full, empty, mutex;

    int in, out;

} buffer_t;

#endif