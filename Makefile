CC = gcc
CFLAGS = -Wall -Wextra -Iplatform $(shell sdl2-config --cflags)
LDLIBS = $(shell sdl2-config --libs)

SRCS = main.c platform/platform.c
OBJS = $(SRCS:.c=.o)
TARGET = chip8

$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LDLIBS)

%.o: %.c platform/platform.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

run: $(TARGET)
	./$(TARGET)

.PHONY: clean run