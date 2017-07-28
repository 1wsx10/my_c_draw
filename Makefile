SOURCES=drawlib.c
OBJECTS=drawlib.o
HEADERS=drawlib.h
PROGRAM=a.out
FLAGS= -ansi -lm -pedantic -Wall -g

all: $(PROGRAM)

%.o: %.c $(HEADERS)
	gcc $(FLAGS) -c $< -o $@

$(PROGRAM): $(OBJECTS)
	gcc $(FLAGS) -o $@ $^

.PHONY:clean archive

clean:
	rm $(PROGRAM) $(OBJECTS)

archive:
	zip $(USER)-a2 $(SOURCES) $(HEADERS) Makefile

