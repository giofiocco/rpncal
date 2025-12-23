MODULES=$(patsubst %.c,%.so, $(wildcard modules/*.c))
CFLAGS=-Wall -Wextra -g

.PHONY: all clean
all: $(MODULES) rpncal

rpncal: rpncal.c
	$(CC) $(CFLAGS) -o $@ $< -lncurses -lm

%.so: %.c module.h
	$(CC) $(CFLAGS) -c -fpic -o $@.o $<
	$(CC) $(CFLAGS) -shared -o $@ $@.o

clean:
	rm -f rpncal modules/*.o modules/*.so
