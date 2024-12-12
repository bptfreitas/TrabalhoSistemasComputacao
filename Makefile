
MATRIX_DIM=1000

CFLAGS = -Iinclude -I. -pthread -fopenmp -DMATRIX_DIM=$(MATRIX_DIM)

LDFLAGS = -pthread -fopenmp

VPATH = include src controller

SRC_1 = consumidor.c produtor.c cp1.c cp2.c cp3.c main.c libaux.c thread_controller.c

SRC_2 = controller.c

OBJ_1 = $(SRC_1:.c=.o)

OBJ_2 = $(SRC_2:.c=.o)

HEADERS = $(SRC_1:.c=.h) $(SRC_2:.c=.h)

BIN_1 = trabalho-sistemas-computacao 

BIN_2 = thread-controller

%.o: %.c $(shell ls include/*.h)
	gcc $(CFLAGS) -c $< -o $@

.PHONY: all clean distclean

all: $(OBJ_1) $(OBJ_2)
	gcc $(OBJ_1) $(LDFLAGS) -o $(BIN_1)
	gcc $(OBJ_2) $(LDFLAGS) -o $(BIN_2)
	chmod +x ./$(BIN_1)	
	chmod +x ./$(BIN_2)

clean:
	rm -f *.o $(BIN) /tmp/matrix_deamon

distclean: clean
	rm -f *.in
