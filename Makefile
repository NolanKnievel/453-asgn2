CC = gcc
CFLAGS = -Wall -fPIC -Werror
LDFLAGS = -shared




lwp.o : lwp.c
	$(CC) $(CFLAGS) -c lwp.c -o lwp.o