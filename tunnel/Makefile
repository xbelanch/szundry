CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb
LIBS=`pkg-config --libs --cflags sdl2`
input=

tunnel: tunnel.c
	$(CC) $(CFLAGS) -o tunnel tunnel.c $(LIBS) -lm