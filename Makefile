
MATRIX_DIM=1000

CFLAGS = -Iinclude -I. -pthread -fopenmp -DMATRIX_DIM=$(MATRIX_DIM)

LDFLAGS = -pthread -fopenmp

VPATH = include src

SRC = consumidor.c produtor.c cp1.c cp2.c cp3.c main.c libaux.c

OBJ = $(SRC:.c=.o)

HEADERS = $(SRC:.c=.h)

BIN = trabalho-sistemas-computacao

%.o: %.c $(shell ls include/*.h)
	gcc $(CFLAGS) -c $< -o $@

.PHONY: all clean distclean

all: $(OBJ)
	gcc $(OBJ) $(LDFLAGS) -o $(BIN)
	chmod +x ./$(BIN)

clean:
	rm -f *.o $(BIN)

distclean: clean
	rm -f *.in
