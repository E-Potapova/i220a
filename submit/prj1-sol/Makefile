CC = gcc
CPPFLAGS = -g -Wall -std=c18
LDFLAGS = -lm

morse-decode:	morse-encode
		ln -s -f $< $@

morse-encode:	file-utils.o main.o morse.o
		$(CC) file-utils.o main.o morse.o $(LDFLAGS) -o $@

depend:
		$(CC) -MM $(CPPFLAGS) *.c

.PHONY:		clean
clean:
		rm -f *~ *.o morse-encode morse-decode

# auto-dependencies create by 'depend'
file-utils.o: file-utils.c file-utils.h
main.o: main.c file-utils.h morse.h tests.h
morse.o: morse.c morse.h tests.h
tests.o: tests.c morse.h tests.h
