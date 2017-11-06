CXX		= g++

OPTS	= -g -O2 -std=c++11
CFLAGS	= $(OPTS)
LIBS	= -lm

TARGET 	= fcd

default: $(TARGET)

clean:
	rm -rf *~ *.o $(TARGET)

fcd: main.cpp fcd.o Community.o Member.o
	$(CXX) $(CFLAGS) -o fcd main.cpp fcd.o Community.o Member.o $(LIBS)

Member.o: Member.cpp Member.h
	$(CXX) $(CFLAGS) -c Member.cpp

Community.o: Community.cpp Community.h
	$(CXX) $(CFLAGS) -c Community.cpp

fcd.o: fcd.cpp fcd.h
	$(CXX) $(CFLAGS) -c fcd.cpp