CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb
LIBS=`pkg-config --libs --cflags sdl2`
input=

tunnel:
	$(CC) $(CFLAGS) -o tunnel tunnel2.c $(LIBS) -lm