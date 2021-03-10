CC = gcc
CFLAGS = -g -O2 -Wall
LFLAGS = -lwiringPi

all:	lab4a lab4b

lab4a:		lab4a.o
	$(CC) $(LFLAGS) -o lab4a lab4a.o

lab4a.o:	lab4a.c
	$(CC) $(CFLAGS) -c lab4a.c

lab4b:		lab4b.o
	$(CC) $(LFLAGS) -o lab4b lab4b.o

lab4b.o:	lab4b.c
	$(CC) $(CFLAGS) -c lab4b.c

clean:
	rm -f *~ *.o lab4a lab4b
