CC=gcc
FLAGS=-Wno-implicit-function-declaration -g
CFLAGS=-I.
OBJ = msh.o 
INC= parser.o
LIBS=-lparser

%.o: %.c 
	$(CC) $(FLAGS) -c -o $@ $< $(CFLAGS)

msh: $(OBJ)
	$(CC) $(FLAGS) -L. -o $@ $< $(LIBS) -Wl,-rpath=.

clean:
	rm -f ./msh.o ./msh
