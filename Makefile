CC=gcc

.PHONY: chip8 flags

chip8: chip8.c platform.c
	# $(CC) chip8.c platform.c -Wall -Wpedantic -o chip8 $(LIBS) 
	$(CC) chip8.c platform.c -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL2 -Wall -Wpedantic -o chip8


flags: chip8.c platform.c
	# $(CC) chip8.c platform.c -Wall -Wpedantic -o chip8 $(LIBS) 
	$(CC) chip8.c platform.c -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL2 -Wall -Wpedantic -o chip8
	./chip8 10 5 tests/flags.ch8