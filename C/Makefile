CC=gcc
CFLAGS=-Wall -Werror -Wextra -Wstrict-prototypes -Wmissing-prototypes -Wpointer-arith -Wreturn-type -Wcast-qual -Wswitch -Wshadow -Wcast-align -Wno-unused-parameter -Wchar-subscripts -Winline -Wnested-externs -Wredundant-decls
SOURCES=test-dkim-regdom.c dkim-regdom.c dkim-regdom.h tld-canon.h
EXECUTABLE=test-dkim-regdom

all: $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)	

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(LDFLAGS)
