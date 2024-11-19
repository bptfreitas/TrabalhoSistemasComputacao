#!/bin/python3

import sys
import random

try:
    MATRIX_DIM=int( sys.argv[1] )
except:
    MATRIX_DIM=2

print("Matrix dimensions:", (MATRIX_DIM, MATRIX_DIM))

for file_index in range(10):

    matrix = [ ]

    with open( "entrada" + str(file_index) + ".in" , "w") as entrada:

        # 1st matrix: elements are (i+1)* j

        for i in range( MATRIX_DIM ):

            line = []
            
            for j in range( MATRIX_DIM ):

                line.append( str( (i + 1) * j ) )

            matrix.append( line )

            entrada.write( ' '.join( line ) + "\n" )

        # 2nd matrix: elements are (j+1)

        for i in range( MATRIX_DIM ):

            line = []
            
            for j in range( MATRIX_DIM ):

                line.append( str( 1 + j ) )

            matrix.append( line )

            entrada.write( ' '.join( line ) + "\n" )            

        



