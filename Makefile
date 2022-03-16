CC=gcc
CFLAGS=-Wall -Wextra -g -O0
LDFLAGS=-lncurses
VGFLAGS=--track-origins=yes
#VGFLAGS += --leak-check=full

all: test

build/help.o: help.c help.h
	$(CC) $(CFLAGS) -c -o $@ $<

build/test.o: test.c help.h
	$(CC) $(CFLAGS) -c -o $@ $<

test: build/help.o build/test.o
	$(CC) -o $@ $^ $(LDFLAGS)

vg: test
	@-valgrind $(VGFLAGS) ./test 2> err
	@cat err

run: test
	@-./test 2> err
	@cat err

vgclean:
	@echo "Removing valgrind core dumps"
	@rm -f vgcore.*

clean: vgclean
	@echo "Removing build and output files"
	@rm -f build/*.o test err

.PHONY: vg run vgclean clean
