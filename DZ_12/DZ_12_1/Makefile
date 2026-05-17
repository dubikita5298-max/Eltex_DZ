TARGET_1 = DZ_12_1
CC = gcc
CFLAGS = -g -Wall -Wextra

all: $(TARGET_1)

$(TARGET_1): DZ_12_1.o
	$(CC) $(CFLAGS) DZ_12_1.o -o $(TARGET_1)

DZ_12_1.o: DZ_12_1.c
	$(CC) $(CFLAGS) -c DZ_12_1.c

.PHONY: clean

clean: 
	rm -f *.o $(TARGET_1) 