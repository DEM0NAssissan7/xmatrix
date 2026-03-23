CC=gcc
CFLAGS = -Wall -Wextra -lm
BUILD_DIR = build

all: linux

main: matrix.c matrix.h number.c number.h mathlib.c mathlib.h
	$(CC) $(CFLAGS) -c matrix.c
	$(CC) $(CFLAGS) -c number.c
	$(CC) $(CFLAGS) -c mathlib.c

linux: main main-linux.c
	$(CC) $(CFLAGS) -c main-linux.c
	$(CC) $(CFLAGS) -o xmatrix-linux main-linux.o matrix.o number.o mathlib.o

clean:
	rm -f *.o xmatrix-linux