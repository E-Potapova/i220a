CFLAGS = -g -Wall -std=c18  -fPIC

TARGETS = fns 
all:		$(TARGETS)

#Target used to build shared object module containing functions used
#for testing.  Presupposes existence of fns.c (which could have been
#generated automatically).
fns:		fns.c
		$(CC) $(CFLAGS) -shared $< -o $@

clean:
		rm -f *~ $(TARGETS)
