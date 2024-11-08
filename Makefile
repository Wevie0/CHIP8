CC=gcc
CFLAGS=-o

LIBS=-lSDL2

.PHONY: chip8 flags

chip8: chip8.c platform.c
	$(CC) chip8.c platform.c -Wall -Wpedantic -o chip8 $(LIBS) 

flags: chip8.c platform.c
	$(CC) chip8.c platform.c -Wall -Wpedantic -o chip8 $(LIBS) 
	./chip8 1 1 tests/flags.ch8m