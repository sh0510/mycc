CFLAGS=-std=c11 -g0 -O0 -static
CC=cc
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

# If 'make DEBUG=1', add -DDEBUG_PRINT_TOKENS to CFLAGS for enabling debug.
DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS := -DDEBUG_PRINT_TOKENS
endif

mycc: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJS): mycc.h

test: mycc
	./test.sh

clean:
	rm -f $(TARGET) $(OBJS) *~ tmp*
.PHONY: test clean
