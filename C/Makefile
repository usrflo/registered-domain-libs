CC=gcc
CFLAGS=-Wall -Werror -Wextra -Wstrict-prototypes -Wmissing-prototypes -Wpointer-arith -Wreturn-type -Wcast-qual -Wswitch -Wshadow -Wcast-align -Wno-unused-parameter -Wchar-subscripts -Winline -Wnested-externs -Wredundant-decls
SOURCES=test-dkim-regdom.c dkim-regdom.c dkim-regdom.h tld-canon.h
EXECUTABLE=test-dkim-regdom
OBJECTS=dkim-regdom.o
LIBRARY=libregdom.so
PREFIX=/usr/local

all: $(EXECUTABLE) $(LIBRARY)

clean:
	rm -f $(EXECUTABLE)
	rm -f $(LIBRARY)
	rm -f $(OBJECTS)

install: libregdom.so
	cp libregdom.so $(PREFIX)/lib
	cp dkim-regdom.h $(PREFIX)/include

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(LDFLAGS)

dkim-regdom.o: dkim-regdom.c dkim-regdom.h
	$(CC) $(CFLAGS) -fPIC -o dkim-regdom.o -c dkim-regdom.c

$(LIBRARY): $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS)
