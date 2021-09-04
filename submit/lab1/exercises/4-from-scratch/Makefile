PROJECT = key-value
SRC_FILES = \
	Makefile \
	key-value.c \
	key-value.h \
	main.c \
	process-file.c \
	process-file.h \
	README
CFLAGS = -g -Wall -std=c18
LDFLAGS = -lm

$(PROJECT):	key-value.o main.o process-file.o
		$(CC) key-value.o main.o process-file.o $(LDFLAGS) -o $@

clean:
		rm -f *.o $(PROJECT) $(PROJECT).tar.gz

depend:
		$(CC) -MM $(CPPFLAGS) *.c

dist:		$(PROJECT).tar.gz

$(PROJECT).tar.gz:	$(SRC_FILES)
		tar -cf $(PROJECT).tar $(SRC_FILES)
		gzip -f $(PROJECT).tar

#dependencies automatically created by depend
key-value.o: key-value.c key-value.h
main.o: main.c process-file.h
process-file.o: process-file.c key-value.h process-file.h
