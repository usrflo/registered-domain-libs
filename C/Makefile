
CC     = gcc
CFLAGS = -g -O2 -std=c99 \
	 -pedantic -Wall -Wextra -Werror -Wwrite-strings \
	 -Wstrict-prototypes -Wmissing-prototypes -Wnested-externs \
	 -Wredundant-decls -Wcast-qual -Wcast-align -Winline \
	 -Wno-unused-parameter -Wno-overlength-strings

SOURCES    = test-regdom.c regdom.c regdom.h tld-canon.h
EXECUTABLE = test-regdom
OBJECTS    = regdom.o test-regdom.o
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

install: $(LIBRARY) regdom.h
	cp $(LIBRARY) $(libdir)
	cd $(libdir) && ln -s $(LIBRARY) libregdom.so
	cp regdom.h $(includedir)

$(EXECUTABLE): test-regdom.o $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ $^

$(LIBRARY): regdom.o
	$(CC) -shared -Wl,-soname,$@ -o $@ $^

test-regdom.o: test-regdom.c regdom.h
regdom.o: regdom.c regdom.h tld-canon.h
regdom.o: CFLAGS += -fPIC
