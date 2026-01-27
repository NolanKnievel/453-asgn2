CC = gcc
CFLAGS = -Wall -Werror -fPIC
# SUPPLIED = ~pn-cs453/Given/Asgn2
SUPPLIED = //home/pn-cs453/Given/Asgn2/demos
OBJS = lwp.o scheduler.o magic64.o



liblwp.so : lwp.o scheduler.o magic64.o
	$(CC) -shared lwp.o scheduler.o magic64.o -o liblwp.so

lwp.o : lwp.c lwp.h
	$(CC) $(CFLAGS) -c lwp.c -o lwp.o

scheduler.o : scheduler.c scheduler.h
	$(CC) $(CFLAGS) -c scheduler.c -o scheduler.o

magic64.o : magic64.S
	gcc -o magic64.o -c magic64.S

test_scheduler.o : test_scheduler.c lwp.h scheduler.h
	$(CC) $(CFLAGS) -c test_scheduler.c -o test_scheduler.o

test_scheduler : test_scheduler.o liblwp.so
	$(CC) -Wall -Werror test_scheduler.o -L. -llwp -o test_scheduler

test.o : test.c lwp.h scheduler.h
	$(CC) $(CFLAGS) -c test.c -o test.o

test : test.o liblwp.so
	$(CC) -Wall -Werror test.o -L. -llwp -o test

# Wandering Snakes demo
randomsnakes: randomsnakes.o liblwp.so
	$(CC) $(CFLAGS) randomsnakes.o -L. -llwp -o randomsnakes -lm -lncurses

randomsnakes.o : randomsnakes.c
	$(CC) $(CFLAGS) -c randomsnakes.c -o randomsnakes.o


# Indented numbers demo
numbersmain: numbersmain.o liblwp.so
	$(CC) $(CFLAGS) numbersmain.c -L. -llwp -o numbersmain -lm

numbersmain.o : numbersmain.c
	$(CC) $(CFLAGS) -c numbersmain.c -o numbersmain.o




clean:
	rm -f *.o *.so test_scheduler
