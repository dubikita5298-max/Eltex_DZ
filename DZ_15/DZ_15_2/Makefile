TARGET_1 = sig_2
TARGET_2 = send_2
CC = gcc
CFLAGS = -g -Wall -Wextra

all: $(TARGET_1) $(TARGET_2)

$(TARGET_1): sig_2.o
	$(CC) $(CFLAGS) sig_2.o -o $(TARGET_1)

sig_2.o: sig_2.c
	$(CC) $(CFLAGS) -c sig_2.c

$(TARGET_2): send_2.o
	$(CC) $(CFLAGS) send_2.o -o $(TARGET_2)

client.o: send_2.c
	$(CC) $(CFLAGS) -c send_2.c

.PHONY: clean

clean: 
	rm -f *.o $(TARGET_1) $(TARGET_2)