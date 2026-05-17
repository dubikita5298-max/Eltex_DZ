TARGET_1 = server
TARGET_2 = client_1
TARGET_3 = client_2
TARGET_4 = client_3
CC = gcc
CFLAGS = -g -Wall -Wextra
LIBS = -lncurses

all: $(TARGET_1) $(TARGET_2) $(TARGET_3) $(TARGET_4)

$(TARGET_1): server.o srv_func.o
	$(CC) $(CFLAGS) server.o srv_func.o -o $(TARGET_1)

server.o: server.c inface.h
	$(CC) $(CFLAGS) -c server.c

srv_func.o: srv_func.c inface.h
	$(CC) $(CFLAGS) -c srv_func.c

$(TARGET_2): client_1.o graphics.o
	$(CC) $(CFLAGS) client_1.o graphics.o -o $(TARGET_2) $(LIBS)

client_1.o: client_1.c inface.h
	$(CC) $(CFLAGS) -c client_1.c

graphics.o: graphics.c inface.h
	$(CC) $(CFLAGS) -c graphics.c

$(TARGET_3): client_2.o graphics.o
	$(CC) $(CFLAGS) client_2.o graphics.o -o $(TARGET_3) $(LIBS)

client_2.o: client_2.c inface.h
	$(CC) $(CFLAGS) -c client_2.c

$(TARGET_4): client_3.o graphics.o
	$(CC) $(CFLAGS) client_3.o graphics.o -o $(TARGET_4) $(LIBS)

client_3.o: client_3.c inface.h
	$(CC) $(CFLAGS) -c client_3.c


clean: 
	rm -f *.o $(TARGET_1) $(TARGET_2) $(TARGET_3) $(TARGET_4) key_namelst.txt key_mesg_hstr.txt