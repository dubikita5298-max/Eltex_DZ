KEY = -g -Wall
all: calc
DELETE = add.o sub.o mul.o div.o

calc: libfunc.so main.c
	gcc main.c -o calc $(KEY) -L. -lfunc 

libfunc.so: add.o sub.o mul.o div.o
	gcc -shared add.o sub.o mul.o div.o -o libfunc.so

add.o: add.c
	gcc -c add.c -fPIC $(KEY) 

sub.o: sub.c
	gcc -c sub.c -fPIC $(KEY) 

mul.o: mul.c
	gcc -c mul.c -fPIC $(KEY)  

div.o: div.c
	gcc -c div.c -fPIC $(KEY)  
clean:
	rm -f $(DELETE)