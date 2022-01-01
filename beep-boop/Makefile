CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb
LIBS=`pkg-config --cflags --libs sdl2`
INPUT=
main: main.c
	$(CC) $(CFLAGS) -o main main.c -lm $(LIBS)