TARGET_1 = sig_1
TARGET_2 = send_1
CC = gcc
CFLAGS = -g -Wall -Wextra

all: $(TARGET_1) $(TARGET_2)

$(TARGET_1): sig_1.o
	$(CC) $(CFLAGS) sig_1.o -o $(TARGET_1)

sig_1.o: sig_1.c
	$(CC) $(CFLAGS) -c sig_1.c

$(TARGET_2): send_1.o
	$(CC) $(CFLAGS) send_1.o -o $(TARGET_2)

client.o: send_1.c
	$(CC) $(CFLAGS) -c send_1.c

.PHONY: clean

clean: 
	rm -f *.o $(TARGET_1) $(TARGET_2)