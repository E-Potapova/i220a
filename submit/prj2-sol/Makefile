CC = gcc
CPPFLAGS = -g -Wall -std=c18
LDFLAGS = -lm

int-set:	main.o int-set.o int-set-strings.o
		$(CC) main.o int-set.o int-set-strings.o $(LDFLAGS) -o $@

depend:
		$(CC) -MM $(CPPFLAGS) *.c

.PHONY:		clean
clean:
		rm -f *~ *.o int-set

# auto-dependencies create by 'depend'
int-set-strings.o: int-set-strings.c int-set.h int-set-strings.h
int-set.o: int-set.c int-set.h
main.o: main.c int-set.h int-set-strings.h
