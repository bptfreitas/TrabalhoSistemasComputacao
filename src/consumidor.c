#include <consumidor.h>

#include <stdio.h>

void* consumidor(void* args){


    buffer_t *buffers = (buffer_t*)args;

    buffer_t *buffer_do_cp3 = &buffers[3];

    fprintf(stdout, "\nStarting consumer thread ... " );    

   
}
