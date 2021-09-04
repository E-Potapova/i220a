PROJECT = quadr
SRC_FILES = \
	Makefile \
	README \
	test.data \
	quadr.c \
	main.c \
	quadr.h

CFLAGS = -g -Wall -std=c18
LDFLAGS = -lm

$(PROJECT):	quadr.o main.o
		$(CC) quadr.o main.o $(LDFLAGS) -o $@

clean:
		rm -f *.o $(PROJECT) $(PROJECT).tar.gz

depend:
		$(CC) -MM $(CPPFLAGS) *.c

dist:		$(PROJECT).tar.gz

$(PROJECT).tar.gz:	$(SRC_FILES)
		tar -cf $(PROJECT).tar $(SRC_FILES)
		gzip -f $(PROJECT).tar

#automatically generated dependencies produced by make depend
main.o: main.c quadr.h
quadr.o: quadr.c quadr.h
