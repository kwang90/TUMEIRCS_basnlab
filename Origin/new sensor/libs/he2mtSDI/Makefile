CC 	=	g++
CFLAGS 	=	`pkg-config --cflags glib-2.0 rest-0.7 jsoncpp` -fPIC
LIBS 	=	 `pkg-config --libs glib-2.0 rest-0.7 jsoncpp`
OBJ 	= 	$(patsubst %.cpp,%.o,$(wildcard src/*.cpp)) $(patsubst %.cpp,%.o,$(wildcard src/implementations/*.cpp))
INCLUDE = 

all: lib

lib: $(OBJ)
	$(CC) -shared -o libhe2mtSDI.so *.o $(LIBS) $(CFLAGS) -Wl,-rpath=./;

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $<



.PHONY clean, realclean:

clean:
	rm *.o
	

realclean: clean
	rm he2mtSDI.so

