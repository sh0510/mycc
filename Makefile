CFLAGS=-std=c11 -g -static

mycc: mycc.c
test: mycc
	./test.sh

clean:
	rm -f mycc *.0 *~ tmp*
.PHONY: test clean
