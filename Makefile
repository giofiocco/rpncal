all: rpncal $(patsubst %.c,%.so, $(wildcard modules/*.c))
.PHONY: all run clean

CFLAGS=-Wall -Wextra -g

rpncal: rpncal.c
	$(CC) $(CFLAGS) -o $@ $< -lncurses -lm

%.so: %.c module.h
	$(CC) $(CFLAGS) -c -fpic -o $@.o $<
	$(CC) $(CFLAGS) -shared -o $@ $@.o

run: rpncal
	./$<

clean:
	rm rpncal modules/*.o modules/*.so
