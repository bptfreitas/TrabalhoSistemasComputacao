#include <cp1.h>

#include <defs.h>

#include <stdio.h>
#include <string.h>

#include <stdlib.h>


void *cp1( void* args ){

    buffer_t *buffers = (buffer_t*)args;

    buffer_t *buffer_do_produtor = &buffers[0];
    
    buffer_t *buffer_para_cp2 = &buffers[1];

    fprintf(stdout, "\nStarting CP1 thread ... " );


    
    




}
