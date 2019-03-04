CFLAGS+=	-std=iso9899:2011 \
		-D_XOPEN_SOURCE	\
		-Wall \
		-Os
LDFLAGS+=	-Os \
		-static

DESTDIR?=	/usr/local
BINDIR?=	/bin

all: p vis
.PHONY: all

vis: vis.o
	${LD} ${LDFLAGS} -o $@ vis.o

p: p.o
	${LD} ${LDFLAGS} -o $@ p.o

install: p vis
	install -s p vis ${DESTDIR}${BINDIR}

clean:
	rm -f *.o p vis
.PHONY: clean
