TARGET_1 = server
TARGET_2 = client
CC = gcc
CFLAGS = -g -Wall -Wextra

all: $(TARGET_1) $(TARGET_2)

$(TARGET_1): server.o
	$(CC) $(CFLAGS) server.o -o $(TARGET_1)

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

$(TARGET_2): client.o
	$(CC) $(CFLAGS) client.o -o $(TARGET_2)

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

.PHONY: clean

clean: 
	rm -f *.o $(TARGET_1) $(TARGET_2)