# TODO: rewrite with library .a, remove unnecessary linking
all:
	gcc -lglut -lGLU -lGLEW -Iinclude src/*.c examples/hello.c -g -o bin/hello
