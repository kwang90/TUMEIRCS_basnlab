SOURCES = $(wildcard ../src/*.c) $(wildcard *.c)	#see http://www.gnu.org/software/make/manual/html_node/Wildcard-Function.html
OBJECTS = $(patsubst %.c,%.o, $(SOURCES))		#see http://www.gnu.org/software/make/manual/html_node/Wildcard-Function.html
CC = gcc
CFLAGS = -lm

.phony: clean, realclean

all: examples

examples: $(OBJECTS)
	$(CC)  $(OBJECTS) $(CFLAGS) -o examples 

clean: 
	rm *.o ../src/*.o

realclean: clean
	rm examples
