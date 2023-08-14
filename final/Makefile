CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lsodium
TARGET = main
SOURCES = main.c mymodel.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET)
