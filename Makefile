MODULES=$(patsubst %.c,%.so, $(wildcard modules/*.c))

all: $(MODULES) rpncal
.PHONY: all clean

CFLAGS=-Wall -Wextra -g

rpncal: rpncal.c
	$(CC) $(CFLAGS) -o $@ $< -lncurses -lm

%.so: %.c module.h
	$(CC) $(CFLAGS) -c -fpic -o $@.o $<
	$(CC) $(CFLAGS) -shared -o $@ $@.o

clean:
	rm -f rpncal modules/*.o modules/*.so
