#include <consumidor.h>

#include <stdio.h>

#include <string.h>

void* consumidor(void* args){


    buffer_t *buffers = (buffer_t*)args;

    buffer_t *buffer_do_cp3 = &buffers[3];

    fprintf(stdout, "\nStarting consumer thread ... " );

    while (1){

        sem_wait( &buffer_do_cp3->empty );

        S_t *data;

        int out;

        sem_wait( &buffer_do_cp3->mutex );    

        out = buffer_do_cp3->out;

        data = buffer_do_cp3->data[ out % BUFFER_SIZE ];

        buffer_do_cp3->data[ out % BUFFER_SIZE ] = NULL;

        buffer_do_cp3->out ++;

        sem_post( &buffer_do_cp3->mutex );

        char output_filename[256];

        strcpy( output_filename, data->source_filename);

        strcat( data->source_filename, "saida.out" );

        FILE *output_fd = fopen( output_filename, "w");

        if (output_fd == NULL){

            perror("Error:");

            continue;

        }
        

        // Close file after use

        fclose( output_fd );

    }

   
}
