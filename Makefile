

CFLAGS = -Iinclude -I. -pthread -fopenmp

LDFLAGS = -pthread -fopenmp

VPATH = include src

SRC = consumidor.c produtor.c main.c

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
	rm *.o $(BIN)

distclean: clean
	rm -f *.in