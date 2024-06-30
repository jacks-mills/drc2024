C := gcc
CFLAGS := -Wall -pedantic -std=gnu99 -g

TARGETS := start control

.PHONY: all clean

all: $(TARGETS)

clean:
	rm -f $(TARGETS) *.o

start: start.o
	$(CC) $(CFLAGS)   -o $@ $^

control: control.o
	$(CC) $(CFLAGS)   -o $@ $^
