CC = g++
LFLAGS = -ggdb3 -std=c++0x -Wall -Werror
TFLAGS = -pthread -lm
EXEC = proxy
OBJ = cache.o cacheinstance.o commclient.o commserver.o httpheader.o httpbody.o

proxy : $(OBJ) main.cpp
	$(CC) $(LFLAGS) $(OBJ) main.cpp -o $(EXEC) $(TFLAGS)

cache.o: cache.hpp cacheinstance.hpp
	$(CC) $(LFLAGS) -c cache.cpp

cacheinstance.o: cacheinstance.hpp httpheader.hpp httpbody.hpp
	$(CC) $(LFLAGS) -c cacheinstance.cpp

commclient.o: commclient.hpp commclient.cpp
	$(CC) $(LFLAGS) -c commclient.cpp

commserver.o: commserver.hpp
	$(CC) $(LFLAGS) -c commserver.cpp

httpheader.o: httpheader.hpp
	$(CC) $(LFLAGS) -c httpheader.cpp

httpbody.o: httpbody.hpp
	$(CC) $(LFLAGS) -c httpbody.cpp

# parse.o: parse.hpp
# 	$(CC) $(LFLAGS) -c parse.hpp

clean:
	rm -f *.o $(EXEC) *~ *.gch
