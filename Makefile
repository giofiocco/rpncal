all: rpncal module.so
.PHONY: all

CFLAGS=-Wall -Wextra

rpncal: main.c
	$(CC) $(CFLAGS) -o $@ $< -lncurses

module.so: module.c module.h
	$(CC) $(CFLAGS) -c -fpic -o $@.o $<
	$(CC) $(CFLAGS) -shared -o $@ $@.o

.PHONY: run
run: rpncal
	./$<
