CC=gcc
CFLAGS=-std=c99 -D_POSIX_C_SOURCE

all: rinfo.c rinfod.c
	$(CC) $(CFLAGS) -c rinfo.c
	$(CC) $(CFLAGS) -o rinfo rinfo.o
	$(CC) $(CFLAGS) -c rinfod.c
	$(CC) $(CFLAGS) -o rinfod rinfod.o

clean:
	rm *.o rinfo rinfod
