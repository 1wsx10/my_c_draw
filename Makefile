SOURCES=drawlib.c
OBJECTS=drawlib.o
HEADERS=drawlib.h
PROGRAM=a.out
CFLAGS= -lm -pedantic -Wall
LDFLAGS= -lm -pedantic -Wall

all: $(PROGRAM)

%.o: %.c $(HEADERS)
	gcc $(CFLAGS) -c $< -o $@

$(PROGRAM): $(OBJECTS)
	gcc $(LDFLAGS) -o $@ $^

.PHONY:clean archive debug

debug:clean all
debug:
CFLAGS+= -fsanitize=address -g

debug:
LDFLAGS+= -fsanitize=address -g

debug:
%.o: %.c $(HEADERS)
	gcc $(CFLAGS) -c $< -o $@

debug:
$(PROGRAM): $(OBJECTS)
	gcc $(LDFLAGS) -o $@ $^


clean:
	rm $(PROGRAM) $(OBJECTS)

archive:
	zip $(USER)-a2 $(SOURCES) $(HEADERS) Makefile

