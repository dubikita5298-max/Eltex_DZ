TARGET = DZ_12_3
CC = gcc
CFLAGS = -g -Wall -Wextra

all: $(TARGET)

$(TARGET): comand.o func.o
	$(CC) $(CFLAGS) comand.o func.o -o $(TARGET)

comand.o: comand.c int_face.h
	$(CC) $(CFLAGS) -c comand.c

func.o: func.c int_face.h
	$(CC) $(CFLAGS) -c func.c

.PHONY: clean

clean: 
	rm -f *.o $(TARGET)