CC = gcc
CFLAGS = -g

all: finds my_printf

finds: finds.c apue.h error.o pathalloc.o pathalloc.h
	$(CC) $(CFLAGS) -o finds error.o pathalloc.o finds.c

my_printf: my_printf.c
	$(CC) $(CFLAGS) -o my_printf my_printf.c

pathalloc.o: pathalloc.c pathalloc.h error.h
	$(CC) -c pathalloc.c -o pathalloc.o

error.o: error.c error.h
	$(CC) $(CFLAGS) -c error.c -o error.o
clean:
	rm -f *.o *~
