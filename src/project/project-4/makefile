CC=gcc
CFLAGS=

default: evenodd.c driver.c scanner.c
	$(MAKE) driver
	$(MAKE) scanner
	$(MAKE) evenodd

driver: driver.c
	$(CC) $(CFLAGS) -g -c driver.c
	$(CC) $(CFLAGS) -g -o driver driver.o

scanner: scanner.c
	$(CC) $(CFLAGS) -g -c scanner.c
	$(CC) $(CFLAGS) -g -o scanner scanner.o

evenodd: evenodd.c
	$(CC) $(CFLAGS) -g -c evenodd.c
	$(CC) $(CFLAGS) -g -o even evenodd.o
	$(CC) $(CFLAGS) -g -o odd evenodd.o

clean:
	rm *.o driver scanner even odd