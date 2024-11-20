#include <stdio.h>

#include <defs.h>

#include <pthread.h>

#include <produtor.h>

#include <cp1.h>
#include <cp2.h>
#include <cp3.h>

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
    
    int index = 0;

    pthread_create( &thread_id[ index ], NULL, produtor, &shared );
    index ++;

    for ( int i = 0; i < N_CP1; i++){

        pthread_create( &thread_id[ index ], NULL, cp1, &shared );
        index ++ ;

    }

    for ( int i = 0; i < N_CP2; i++){

        pthread_create( &thread_id[ index ], NULL, cp2, &shared );
        index ++ ;

    }

    for ( int i = 0; i < N_CP3; i++){

        pthread_create( &thread_id[ index ], NULL, cp3, &shared );
        index ++ ;

    }

    for ( int i = 0; i < N_CONSUMIDORES; i++){

        pthread_create( &thread_id[ index ], NULL, consumidor, &shared );
        index ++ ;

    }    

    for ( int i = 0; i< index; i++){

        pthread_join( thread_id[ i ], NULL);
    
    }

    return 0;
}