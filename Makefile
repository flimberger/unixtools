P=p vis
A=amd64
D=$(HOME)/bin/$A

CFLAGS=-std=iso9899:2011 -D_XOPEN_SOURCE -Wall -Os
LDFLAGS=-Os -static

all:	$P

vis:	vis.o
	$(CC) $(LDFLAGS) -o $@ $@.o

p:	p.o
	$(CC) $(LDFLAGS) -o $@ $@.o

install:	$P
	install -s $P $D

clean:
	rm -f *.o $P

.PHONY:	all clean install
