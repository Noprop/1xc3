CC = gcc
CFLAGS = -Wall -Wextra -std=c99 $(shell pkg-config --cflags libsodium)
LDFLAGS = $(shell pkg-config --libs libsodium)

all: main

main: main.o mymodel.o
	$(CC) $(CFLAGS) main.o mymodel.o -o main $(LDFLAGS)

main.o: main.c mymodel.h constants.h
	$(CC) $(CFLAGS) -c main.c -o main.o

mymodel.o: mymodel.c mymodel.h constants.h
	$(CC) $(CFLAGS) -c mymodel.c -o mymodel.o

clean:
	rm -f main *.o
