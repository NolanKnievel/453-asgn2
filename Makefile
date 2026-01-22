CC = gcc
CFLAGS = -Wall -Werror -fPIC
LDFLAGS =

liblwp.so : lwp.o
	$(CC) -shared lwp.o -o liblwp.so

lwp.o : lwp.c lwp.h
	$(CC) $(CFLAGS) -c lwp.c -o lwp.o

test_scheduler : test_scheduler.c liblwp.so
	$(CC) -Wall -Werror test_scheduler.c -L. -llwp -o test_scheduler

clean:
	rm -f *.o *.so test_scheduler

