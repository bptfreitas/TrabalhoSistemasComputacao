#!/bin/python3

import sys
import random

try:
    MATRIX_DIM=int( sys.argv[1] )
except:
    MATRIX_DIM=10
    
try:
	N_MATRIXES = int( sys.argv[2] )
except:
	N_MATRIXES = 50

print("Matrix dimensions:", (MATRIX_DIM, MATRIX_DIM))
print("Number of matrixes:", N_MATRIXES )

for file_index in range( N_MATRIXES ):

    matrix = [ ]

    with open( "matrixes" + str(file_index) + ".in" , "w") as entrada:

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

    with open("entrada.in", "w") as entrada:

        for index in range(N_MATRIXES):
            entrada.write( "matrixes" + str(index) + ".in" )

            if index < N_MATRIXES:
                entrada.write("\n")
        

          

        



