CC=gcc

CFLAGS = -ansi -c -w -Wall -pedantic -D_GNU_SOURCE

all: mcast message_list recv_dbg test 

message_list:
	$(CC) -c message_list.c

recv_dbg:
	$(CC) -c recv_dbg.c

mcast: mcast.o message_list.o recv_dbg.o
	$(CC) -o mcast mcast.o message_list.o recv_dbg.o

test: test.o message_list.o
	$(CC) -o test test.c message_list.o

clean:
	rm *.o
	rm mcast

	rm test

%.o:    %.c
	$(CC) $(CFLAGS) $*.c