CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

SRC = $(wildcard src/*.c)
OBJS = $(SRC:.c=.o)
TARGET = program

all: build

build: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

run:
	./$(TARGET) $(INPUT) $(OUTPUT)

debug: build
	gdb ./$(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
