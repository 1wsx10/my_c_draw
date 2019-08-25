SOURCES=drawlib.c main.c
OBJECTS=drawlib.o main.o
HEADERS=drawlib.h
PROGRAM=a.out

#TODO remove -g when building properly
CFLAGS= -lm -pedantic -Wall -g
LDFLAGS= -lm -pedantic -Wall -g

all: $(PROGRAM)

%.o: %.c $(HEADERS)
	gcc $(CFLAGS) -c $< -o $@

$(PROGRAM): $(OBJECTS)
	gcc -o $@ $^ $(LDFLAGS)

.PHONY: clean library all archive debug

library: libdraw.a

libdraw.a: drawlib.o
	ar rcs libdraw.a drawlib.o


debug: clean all
debug: CFLAGS+= -fsanitize=address
debug: LDFLAGS+= -fsanitize=address

clean:
	-rm *.a *.o $(PROGRAM)

archive:
	zip $(USER)-a2 $(SOURCES) $(HEADERS) Makefile

