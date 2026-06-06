TARGET_1 = sig_3
TARGET_2 = send_3
CC = gcc
CFLAGS = -g -Wall -Wextra

all: $(TARGET_1) $(TARGET_2)

$(TARGET_1): sig_3.o
	$(CC) $(CFLAGS) sig_3.o -o $(TARGET_1)

sig_3.o: sig_3.c
	$(CC) $(CFLAGS) -c sig_3.c

$(TARGET_2): send_3.o
	$(CC) $(CFLAGS) send_3.o -o $(TARGET_2)

client.o: send_3.c
	$(CC) $(CFLAGS) -c send_3.c

.PHONY: clean

clean: 
	rm -f *.o $(TARGET_1) $(TARGET_2)