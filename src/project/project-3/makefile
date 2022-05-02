CC=gcc
CFLAGS=-std=c99
LINKS=-lssl -lcrypto

default: vsh.c
	$(CC) $(CFLAGS) -c vsh.c
	$(CC) $(CFLAGS) -o vsh vsh.o $(LINKS)

clean:
	rm *.o vsh