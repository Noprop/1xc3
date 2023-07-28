CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = main
SOURCES = main.c functions.c readFile_randWord.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)