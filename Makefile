CC=gcc
CFLAGS=-o

LIBS=-lSDL2

build: chip8.c 
	$(CC) -Wall -Wpedantic -o chip8 chip8.c $(LIBS) 