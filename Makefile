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
snakemain: snakemain.c lwp.c
	$(CC) $(CFLAGS) snakemain.c lwp.o -o snakemain -lncurses -lm

# Indented numbers demo
numbersmain: numbersmain.c lwp.o
	$(CC) $(CFLAGS) numbersmain.c $(OBJS) -o numbersmain -lm




clean:
	rm -f *.o *.so test_scheduler
