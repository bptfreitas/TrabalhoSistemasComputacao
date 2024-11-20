#include <cp2.h>

#include <defs.h>

#include <stdio.h>
#include <string.h>

#include <stdlib.h>

void *cp2( void* args ){

    buffer_t *buffers = (buffer_t*)args;

    buffer_t *buffer_do_cp1 = &buffers[1];
    
    buffer_t *buffer_para_cp2 = &buffers[2];

    fprintf(stdout, "\nStarting CP2 thread ... " );

    




}
