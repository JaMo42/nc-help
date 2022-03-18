CC=gcc
CXX=g++
CFLAGS=-Wall -Wextra -g -O0
LDFLAGS=-lncurses
VGFLAGS=--track-origins=yes
#VGFLAGS += --leak-check=full

all: test test_cpp

build/help.o: help.c help.h
	$(CC) $(CFLAGS) -c -o $@ $<

build/test.o: test.c help.h
	$(CC) $(CFLAGS) -c -o $@ $<

build/test_cpp.o: test.cc help.h
	$(CXX) $(CFLAGS) -c -o $@ $<

test: build/help.o build/test.o
	$(CC) -o $@ $^ $(LDFLAGS)

test_cpp: build/help.o build/test_cpp.o
	$(CXX) -o $@ $^ $(LDFLAGS)

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
