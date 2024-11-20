#include <stdio.h>

#include <defs.h>

#include <pthread.h>

#include <produtor.h>
#include <consumidor.h>

int main(int argc, char **argv){

    buffer_t shared[4];

    for (int i = 0; i < 4; i++ ){

        sem_init( &shared[ i ].full, 0, 0 );

        sem_init( &shared[ i ].mutex, 0, 1 );        

        sem_init( &shared[ i ].empty, 0, BUFFER_SIZE );        

        shared[ i ].in = 0;
        shared[ i ].out = 0;

    }

    pthread_t thread_id[ TOTAL_THREADS ];

    pthread_create( &thread_id[ 0 ], NULL, produtor, &shared );

    pthread_join( thread_id[ 0 ], NULL);

    return 0;
}