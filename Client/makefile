CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system

all: StompBookClubClient
	g++ -pthread -o bin/StompBookClubClient bin/StompBookClubClient.o bin/Book.o bin/Borrow.o bin/connectionHandler.o bin/Receipt.o bin/StompProtocol.o bin/ThreadForServer.o $(LDFLAGS)

StompBookClubClient: bin/StompBookClubClient.o bin/Book.o bin/Borrow.o bin/connectionHandler.o bin/Receipt.o bin/StompProtocol.o bin/ThreadForServer.o
	
bin/StompBookClubClient.o: src/StompBookClubClient.cpp
	g++ -pthread $(CFLAGS) -o bin/StompBookClubClient.o src/StompBookClubClient.cpp

bin/Book.o: src/Book.cpp
	g++ -pthread $(CFLAGS) -o bin/Book.o src/Book.cpp
	
bin/Borrow.o: src/Borrow.cpp
	g++ -pthread $(CFLAGS) -o bin/Borrow.o src/Borrow.cpp

bin/connectionHandler.o: src/connectionHandler.cpp
	g++ -pthread $(CFLAGS) -o bin/connectionHandler.o src/connectionHandler.cpp

bin/Receipt.o: src/Receipt.cpp
	g++ -pthread $(CFLAGS) -o bin/Receipt.o src/Receipt.cpp
	
bin/StompProtocol.o: src/StompProtocol.cpp
	g++ -pthread $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp
	
bin/ThreadForServer.o: src/ThreadForServer.cpp
	g++ -pthread $(CFLAGS) -o bin/ThreadForServer.o src/ThreadForServer.cpp
	
.PHONY: clean
clean:
	rm -f bin/*
