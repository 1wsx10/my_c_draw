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

.PHONY:clean archive debug

debug:clean all
debug: CFLAGS+= -fsanitize=address

debug: LDFLAGS+= -fsanitize=address

debug:
%.o: %.c $(HEADERS)
	gcc $(CFLAGS) -c $< -o $@

debug:
$(PROGRAM): $(OBJECTS)
	gcc -o $@ $^ $(LDFLAGS)


clean:
	rm $(PROGRAM) $(OBJECTS)

archive:
	zip $(USER)-a2 $(SOURCES) $(HEADERS) Makefile

