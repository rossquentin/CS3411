CC=gcc
CFLAGS=-std=c99

default: insert.c delete.c extract.c testops.c
	$(MAKE) ops
	$(MAKE) tests


ops: insert.c delete.c extract.c
	$(CC) $(CFLAGS) -c insert.c
	$(CC) $(CFLAGS) -c delete.c
	$(CC) $(CFLAGS) -c extract.c

tests: testops.c insert.c delete.c extract.c
	$(CC) $(CFLAGS) -o testops testops.c insert.c delete.c extract.c

clean:
	rm insert.o delete.o extract.o testops test.tmp