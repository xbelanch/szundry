CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb
LIBS=`pkgconf --cflags --libs sdl2`
INPUT=

mode7:
	$(CC) $(CFLAGS) -o mode7 mode7.c $(LIBS) -lm