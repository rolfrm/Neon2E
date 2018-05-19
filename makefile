#OPT = -g0 -O4
OPT = -g3 -Og
LIB_SOURCES1 = main.c string_vector.c module.c test.c
LIB_SOURCES = $(addprefix src/, $(LIB_SOURCES1))
CC = gcc
TARGET = neon_engine
LIB_OBJECTS =$(LIB_SOURCES:.c=.o)
LDFLAGS= -L. $(OPT) -Wextra -rdynamic
LIBS= -liron -licydb -ldl -lm
ALL= $(TARGET)
CFLAGS = -Isrc/ -Iinclude/ -std=gnu11 -c $(OPT) -Wall -Wextra -Werror=implicit-function-declaration -Wformat=0 -D_GNU_SOURCE -fdiagnostics-color -Werror -Wextra  -Wwrite-strings -msse4.2 -Werror=maybe-uninitialized -DUSE_VALGRIND -DDEBUG

$(TARGET): $(LIB_OBJECTS)
	$(CC) $(LDFLAGS) $(LIB_OBJECTS) $(LIBS) -o $@

all: $(ALL)

.c.o: $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@ -MMD -MF $@.depends 
depend: h-depend
clean:
	rm -f $(LIB_OBJECTS) $(ALL) src/*.o.depends
.PHONY: test
test: $(TARGET)
	make -f makefile.compiler
	make -f makefile.test test

-include $(LIB_OBJECTS:.o=.o.depends)

install:
	cp neon_engine /usr/bin/neon_engine
	cp src/neon.h /usr/include/neon.h
uninstall:
	rm /usr/bin/neon_engine
	rm /usr/include/neon.h
