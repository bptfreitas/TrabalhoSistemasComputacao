#include <cp3.h>

#include <defs.h>

#include <stdio.h>
#include <string.h>

#include <stdlib.h>

void *cp3( void* args ){

    buffer_t *buffers = (buffer_t*)args;

    buffer_t *buffer_do_cp2 = &buffers[2];
    
    buffer_t *buffer_para_consumidor = &buffers[3];

    fprintf(stdout, "\nStarting CP3 thread ... " );    

}
