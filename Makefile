CC = gcc
CFLAGS = -Wall -Wextra -Iplatform -Icore $(shell sdl2-config --cflags)
LDLIBS = $(shell sdl2-config --libs)

SRCS = main.c platform/platform.c core/chip8.c
OBJS = $(SRCS:.c=.o)
TARGET = chip8

$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: platform/platform.h core/chip8.h
platform/platform.o: platform/platform.h core/chip8.h
core/chip8.o: core/chip8.h

clean:
	rm -f $(TARGET) $(OBJS)

ROM ?= roms/2-ibm-logo.ch8

run: $(TARGET)
	./$(TARGET) $(ROM)

.PHONY: clean run