CFLAGS = -g -Wall -fPIC -std=c18

OBJS = \
  circ.o \
  rect.o

all:		libgeom.so  libgeom.a testgeom-static testgeom-dynamic

libgeom.so:	$(OBJS)
		$(CC) -shared $(OBJS) -o $@

libgeom.a:	$(OBJS)
		ar rcs $@ $(OBJS)

testgeom-static: testgeom.o
		$(CC)  -static testgeom.o -L. -lgeom -o $@

testgeom-dynamic: testgeom.o
		$(CC) testgeom.o -L. -lgeom -o $@

depend:
		$(CC) -MM $(CFLAGS) *.c

.PHONY:		clean
clean:
		rm -f *.o *.so *.a testgeom-*

# auto-dependencies created by 'depend'
circ.o: circ.c geom.h
rect.o: rect.c geom.h
testgeom.o: testgeom.c geom.h
