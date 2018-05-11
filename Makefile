CC = gcc
CFLAGS = -Wall

#The Executable 'port_scanner' depends on the port_scanner object file
port_scanner: port_scanner.o
	$(CC) $(CFLAGS) -o port_scanner port_scanner.o -std=c99 -D_POSIX_SOURCE -lpthread

#Build port_scanner.o (requires port_scanner.o)
port_scanner.o : port_scanner.c
	$(CC) $(CFLAGS) -c port_scanner.c -std=c99 -D_POSIX_SOURCE -lpthread

#Clean: remove all the object files
clean :
	rm port_scanner *.o
