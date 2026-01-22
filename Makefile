CC = gcc
CFLAGS = -Wall -fPIC -Werror
LDFLAGS = -shared




lwp.o : lwp.c lwp.h
	$(CC) $(CFLAGS) -c lwp.c -o lwp.o

test_scheduler : test_scheduler.c lwp.o
	$(CC) $(CFLAGS) test_scheduler.c lwp.o -o test_scheduler $(LDFLAGS)

clean: 
	rm -f *.o