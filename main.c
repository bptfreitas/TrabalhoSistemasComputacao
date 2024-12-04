#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/inotify.h>

#include <defs.h>

#include <produtor.h>

#include <cp1.h>
#include <cp2.h>
#include <cp3.h>

#include <consumidor.h>

char dir_to_monitor[256];
int inotify_fd;

int main(int argc, char **argv){

    buffer_t shared[4];
    int retval;

    if (argc != 2){
        fprintf(stdout, "Usage: %s [directory to monitor]\n", argv[0]);
        return -1;
    }

    // Checking if directory exists and it writable and readable
    retval = access( argv[1] , F_OK| R_OK | W_OK );

    if (retval == -1){
        perror("Error");
        return -1;
    }    

    retval = chdir(argv[1]);

    if (retval == -1){
        perror("Error");
        return -1;
    }

    strcpy( dir_to_monitor, argv[1]);

    inotify_fd = inotify_init();

    int watched_items;

    long int mask = IN_CREATE;

    int wd = inotify_add_watch (inotify_fd, dir_to_monitor , mask);
    if (wd < 0) {
        printf ("Cannot add watch for \"%s\" with event mask %lX", 
            dir_to_monitor,
            mask);
        fflush (stdout);
        perror (" ");

        return -1;
        
    } else {
        watched_items++;
        printf ("Watching %s WD=%d\n", dir_to_monitor, wd);
        printf ("Watching = %d items\n", watched_items);
    }    

    for (int i = 0; i < 4; i++ ){

        sem_init( &shared[ i ].full, 0, BUFFER_SIZE );

        sem_init( &shared[ i ].mutex, 0, 1 );        

        sem_init( &shared[ i ].empty, 0, 0 );        

        shared[ i ].in = 0;
        shared[ i ].out = 0;

    }

    pthread_t thread_id[ TOTAL_THREADS ];
    
    int index = 0;
    
    for ( int i = 0; i < N_PRODUTORES; i++){

        pthread_create( &thread_id[ index ], NULL, produtor, &shared );
        index ++ ;

    }


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

    fprintf(stdout,"\n");

    return 0;
}