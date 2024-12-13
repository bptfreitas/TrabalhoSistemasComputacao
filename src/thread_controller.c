#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <pthread.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <string.h>
#include <stdlib.h>

#include <defs.h>

#include <libaux.h>

#include <thread_controller.h>

#include <cp1.h>
#include <cp2.h>
#include <cp3.h>

extern pthread_mutex_t cp1_thread_count_lock;
extern int cp1_thread_count;

extern pthread_mutex_t cp2_thread_count_lock;
extern int cp2_thread_count;

extern pthread_mutex_t cp3_thread_count_lock;
extern int cp3_thread_count;

int command_fd;

pthread_barrier_t stop_controller_barrier;

extern buffer_t shared[4];

void create_new_thread( char *type, pthread_mutex_t *mutex, int *counter, void* (*fn) (void*) ){

                
}

void* thread_controller(void* args){

    pthread_barrier_init( &stop_controller_barrier, 0, 2 );

    command_fd = open("/tmp/matrix_deamon", O_RDONLY);

    int nbytes_recv;

    pthread_t thread_id;

    pthread_attr_t thread_attr;

    pthread_attr_setdetachstate( &thread_attr, PTHREAD_CREATE_DETACHED);        

    char command[256];

    while (1){

        nbytes_recv = read( command_fd, &command, 256);

        if (nbytes_recv > 0 ){

            strip_newline( command );

            syslog( LOG_INFO, "[Thread controller] Command received: '%s'", command);

            if ( ! strcmp( command, "stop" ) ){

                syslog( LOG_INFO, "[Thread controller] Exiting thread" );

                pthread_barrier_wait( &stop_controller_barrier );

                break;
            }

            if ( ! strcmp( command, "-c cp1" ) ){

                pthread_mutex_lock( &cp1_thread_count_lock );

                if ( cp1_thread_count < MAX_THREADS -1 ){

                    syslog( LOG_INFO, 
                        "[Thread controller] Creating a new CP1 thread.");

                    pthread_create( &thread_id, &thread_attr, cp1, &shared );

                } else {

                    syslog( LOG_WARNING, 
                        "[Thread controller] Can't create new CP1 thread - new thread count greater than maximum (%d > %d)",
                        cp1_thread_count , MAX_THREADS );
                    
                }

                pthread_mutex_unlock( &cp1_thread_count_lock );        
            } 

            if ( ! strcmp( command, "-c cp2" ) ){

                pthread_mutex_lock( &cp2_thread_count_lock );

                if ( cp2_thread_count < MAX_THREADS -1 ){

                    syslog( LOG_INFO, 
                        "[Thread controller] Creating a new CP2 thread.");

                    pthread_create( &thread_id, &thread_attr, cp2, &shared );

                } else {

                    syslog( LOG_WARNING, 
                        "[Thread controller] Can't create new CP2 thread - new thread count greater than maximum (%d > %d)",
                        cp1_thread_count , MAX_THREADS );
                    
                }

                pthread_mutex_unlock( &cp2_thread_count_lock );        
            }


            if ( ! strcmp( command, "-c cp3" ) ){

                pthread_mutex_lock( &cp3_thread_count_lock );

                if ( cp3_thread_count < MAX_THREADS -1 ){

                    syslog( LOG_INFO, 
                        "[Thread controller] Creating a new CP3 thread.");

                    pthread_create( &thread_id, &thread_attr, cp3, &shared );

                } else {

                    syslog( LOG_WARNING, 
                        "[Thread controller] Can't create new CP3 thread - new thread count greater than maximum (%d > %d)",
                        cp3_thread_count , MAX_THREADS );
                    
                }

                pthread_mutex_unlock( &cp3_thread_count_lock );                        
            } 

            if ( ! strcmp( command, "-d cp1" ) ){

                syslog( LOG_INFO, 
                        "[Thread controller] Destroying CP1 thread.");                

                S_t *end_work = (S_t*) malloc( sizeof(S_t) );

                end_work->work_type = WORK_END_THREAD_CP1;

                pass_work( end_work, &shared[0]);

            }

            if ( ! strcmp( command, "-d cp2" ) ){

                syslog( LOG_INFO, 
                        "[Thread controller] Destroying a CP2 thread.");                

                S_t *end_work = (S_t*) malloc( sizeof(S_t) );

                end_work->work_type = WORK_END_THREAD_CP2;

                pass_work( end_work, &shared[1]);

            }

            if ( ! strcmp( command, "-d cp3" ) ){

                syslog( LOG_INFO, 
                        "[Thread controller] Destroying a CP3 thread.");                

                S_t *end_work = (S_t*) malloc( sizeof(S_t) );

                end_work->work_type = WORK_END_THREAD_CP3;

                pass_work( end_work, &shared[2]);

            }            


        }


    }

    pthread_barrier_destroy( &stop_controller_barrier );


}