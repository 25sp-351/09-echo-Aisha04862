CC = gcc
CFLAGS = -Wall -pthread
TARGET = echo_server

all: $(TARGET)

$(TARGET): main.o server.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o server.o

main.o: main.c server.h
	$(CC) $(CFLAGS) -c main.c

server.o: server.c server.h
	$(CC) $(CFLAGS) -c server.c

clean:
	rm -f *.o $(TARGET)