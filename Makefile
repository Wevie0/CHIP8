CC=gcc
CFLAGS=-o

LIBS=-lSDL2

build: chip8.c 
	$(CC) chip8.c platform.c -Wall -Wpedantic -o chip8 $(LIBS) 