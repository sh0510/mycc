CFLAGS=-std=c11 -g0 -O0 -static
CC=cc
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

# If 'make DEBUG_PRINT_TOKENS=1', add -DDEBUG_PRINT_TOKENS to CFLAGS for enabling debug.
DEBUG_PRINT_TOKENS ?= 0
ifeq ($(DEBUG_PRINT_TOKENS), 1)
	CFLAGS := -DDEBUG_PRINT_TOKENS
endif
# make DEBUG_PRINT_NODES=1
DEBUG_PRINT_NODES ?= 0
ifeq ($(DEBUG_PRINT_NODES), 1)
	CFLAGS := -DDEBUG_PRINT_NODES
endif

mycc: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJS): mycc.h

test: mycc
	./test.sh

clean:
	rm -f $(TARGET) $(OBJS) *~ tmp*
.PHONY: test clean
