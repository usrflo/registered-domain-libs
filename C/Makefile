
CC     = gcc
CFLAGS = -g -O2 -std=c99 -D_POSIX_C_SOURCE=200112L \
	 -pedantic -Wall -Wextra -Werror -Wwrite-strings \
	 -Wstrict-prototypes -Wmissing-prototypes -Wnested-externs \
	 -Wredundant-decls -Wcast-qual -Wcast-align -Winline \
	 -Wno-unused-parameter -Wno-overlength-strings

SOURCES    = test-dkim-regdom.c dkim-regdom.c dkim-regdom.h tld-canon.h
EXECUTABLE = test-dkim-regdom
OBJECTS    = dkim-regdom.o
LIBRARY    = libregdom.so

# autoconf-compatible installation directory variables
prefix      = /usr/local
exec_prefix = $(prefix)
libdir      = $(exec_prefix)/lib
includedir  = $(prefix)/include

all: $(EXECUTABLE) $(LIBRARY)

clean:
	rm -f $(EXECUTABLE)
	rm -f $(LIBRARY)
	rm -f $(OBJECTS)

install: libregdom.so
	cp libregdom.so $(libdir)
	cp dkim-regdom.h $(includedir)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(LDFLAGS)

dkim-regdom.o: dkim-regdom.c dkim-regdom.h
	$(CC) $(CFLAGS) -fPIC -o dkim-regdom.o -c dkim-regdom.c

$(LIBRARY): $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS)
