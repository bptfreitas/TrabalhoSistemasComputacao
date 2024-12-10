#ifndef __DEFS_H___

#define __DEFS_H___

#include <semaphore.h>

enum work_t {
    WORK_NORMAL = 1,
    WORK_END_THREAD_CP1 = 2,    
    WORK_END_THREAD_CP2 = 3,
    WORK_END_THREAD_CP3 = 4,   
    WORK_END_THREAD_CONSUMER = 5
};

#define MAX_ERRORS 5

#define MATRIX_LINES MATRIX_DIM
#define MATRIX_COLS MATRIX_DIMy

#define N_PRODUTORES 1
#define N_CP1 3
#define N_CP2 3
#define N_CP3 3
#define N_CONSUMIDORES 1

#define TOTAL_THREADS ( N_PRODUTORES + N_CP1 + N_CP2 + N_CP3 + N_CONSUMIDORES)

#define BUFFER_SIZE 5

typedef struct{

    char source_filename[256];

    double A[ MATRIX_LINES] [MATRIX_COLS];

    double B[ MATRIX_LINES] [MATRIX_COLS];

    double C[ MATRIX_LINES] [MATRIX_COLS];
    
    double V[ MATRIX_COLS ];

    double E;    

    enum work_t work_type;

} S_t;

typedef struct {

    S_t* data[ BUFFER_SIZE ];

    sem_t full, empty, mutex;

    int in, out;
    
} buffer_t;

#endif
