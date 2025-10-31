CFLAGS=-std=c11 -g0 -static
CC=cc
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

mycc: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJS): mycc.h

test: mycc
	./test.sh

clean:
	rm -f mycc *.0 *~ tmp*
.PHONY: test clean
