CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread
all: StompEMIClient
StompEMIClient: bin/ConnectionHandler.o bin/StompEMIClient.o bin/event.o bin/KeyboardInputManager.o bin/ServerInputManager.o bin/StompProtocol.o bin/StompFrame.o
	g++ -o bin/StompEMIClient bin/ConnectionHandler.o bin/StompEMIClient.o bin/event.o bin/KeyboardInputManager.o bin/ServerInputManager.o bin/StompProtocol.o bin/StompFrame.o $(LDFLAGS)

bin/KeyboardInputManager.o: src/KeyboardInputManager.cpp
	g++ $(CFLAGS) -o bin/KeyboardInputManager.o src/KeyboardInputManager.cpp

bin/ServerInputManager.o: src/ServerInputManager.cpp
	g++ $(CFLAGS) -o bin/ServerInputManager.o src/ServerInputManager.cpp

bin/StompFrame.o: src/StompFrame.cpp
	g++ $(CFLAGS) -o bin/StompFrame.o src/StompFrame.cpp

bin/StompProtocol.o: src/StompProtocol.cpp
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/event.o: src/event.cpp
	g++ $(CFLAGS) -o bin/event.o src/event.cpp

bin/StompEMIClient.o: src/StompEMIClient.cpp
	g++ $(CFLAGS) -o bin/StompEMIClient.o src/StompEMIClient.cpp

.PHONY: clean
clean:
	rm -f bin/*
	