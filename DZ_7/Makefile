KEY = -g -Wall
all: calc
DELETE = add.o sub.o mul.o div.o

calc: libfunc.a main.c
	gcc main.c -o calc $(KEY) -L. -lfunc 

libfunc.a: add.o sub.o mul.o div.o
	ar rc libfunc.a add.o sub.o mul.o div.o

add.o: add.c
	gcc -c add.c $(KEY) 

sub.o: sub.c
	gcc -c sub.c $(KEY) 

mul.o: mul.c
	gcc -c mul.c $(KEY) 

div.o: div.c
	gcc -c div.c $(KEY) 
clean:
	rm -f $(DELETE)