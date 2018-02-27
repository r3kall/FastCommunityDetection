CXX		= g++

OPTS	= -g -O2 -std=c++11
CFLAGS	= $(OPTS)
LIBS	= -lm

TARGET 	= fcd

default: $(TARGET)

clean:
	rm -rf *~ *.o $(TARGET)

fcd: main.cpp fcd.o Community.o
	$(CXX) $(CFLAGS) -o fcd main.cpp fcd.o Community.o $(LIBS)

fcd.o: fcd.cpp fcd.h Community.h MaxHeap.h
	$(CXX) $(CFLAGS) -c fcd.cpp

Community.o: Community.cpp Community.h
	$(CXX) $(CFLAGS) -c Community.cpp