all: clean examples

library:
	gcc $(CFLAGS) -c src/*.c -o bin/libnoobgraphics.a

examples: library
	gcc $(CFLAGS) $(LDFLAGS) examples/hello.c -o bin/hello
	gcc $(CFLAGS) $(LDFLAGS) examples/snake.c -o bin/snake
	gcc $(CFLAGS) $(LDFLAGS) examples/tetris.c -o bin/tetris

clean:
	rm -f bin/*

LDFLAGS=-lglut -lGLEW bin/libnoobgraphics.a
CFLAGS=-Iinclude -g
