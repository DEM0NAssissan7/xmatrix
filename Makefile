CC=gcc
CFLAGS = -Wall -Wextra -lm
BUILD_DIR = build

all: linux ti84ce

main: matrix.c matrix.h number.c number.h calculator.c calculator.h
	$(CC) $(CFLAGS) -c matrix.c
	$(CC) $(CFLAGS) -c number.c
	$(CC) $(CFLAGS) -c calculator.c

linux: main main-linux.c
	$(CC) $(CFLAGS) -c main-linux.c
	$(CC) $(CFLAGS) -o icalc-linux main-linux.o matrix.o number.o calculator.o

ti84ce:
	./install_cedev.sh

clean:
	rm -f *.o icalc-linux
	@$(MAKE) -f Makefile.ti84ce clean >/dev/null 2>&1 || true
