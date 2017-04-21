CC=g++
CPPFLAGS=-std=c++11

all: ftrestd ftrest

ftrestd: myServer.cpp
	$(CC) $(CPPFLAGS) myServer.cpp -o ftrestd

ftrest: myClient.cpp
	$(CC) $(CPPFLAGS) myClient.cpp -o ftrest

clean: rm *o ftrestd ftrest
