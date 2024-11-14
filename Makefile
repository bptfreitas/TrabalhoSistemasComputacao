

CFLAGS = .include 

LDFLAGS = -pthread -fopenmp

VPATH = include src

SRC = consumidor.c produtor.c main.c

OBJ = $(SRC:.c=.o)

.PHONY all clean distclean


all: $(OBJ)