
CC     = gcc
CFLAGS = -g -O2 -std=c99 -D_POSIX_C_SOURCE=200112L \
	 -pedantic -Wall -Wextra -Werror -Wwrite-strings \
	 -Wstrict-prototypes -Wmissing-prototypes -Wnested-externs \
	 -Wredundant-decls -Wcast-qual -Wcast-align -Winline \
	 -Wno-unused-parameter -Wno-overlength-strings

SOURCES    = test-dkim-regdom.c dkim-regdom.c dkim-regdom.h tld-canon.h
EXECUTABLE = test-dkim-regdom
OBJECTS    = dkim-regdom.o test-dkim-regdom.o
LIBRARY    = libregdom.so.1

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

install: $(LIBRARY) dkim-regdom.h
	cp $(LIBRARY) $(libdir)
	cd $(libdir) && ln -s $(LIBRARY) libregdom.so
	cp dkim-regdom.h $(includedir)

$(EXECUTABLE): test-dkim-regdom.o $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ $^

$(LIBRARY): dkim-regdom.o
	$(CC) -shared -Wl,-soname,$@ -o $@ $^

test-dkim-regdom.o: test-dkim-regdom.c dkim-regdom.h
dkim-regdom.o: dkim-regdom.c dkim-regdom.h tld-canon.h
dkim-regdom.o: CFLAGS += -fPIC
