#include "../include/StompEMIClient.h"
#include "../include/ConnectionHandler.h"
#include "../include/ServerInputManager.h"
#include "../include/KeyboardInputManager.h"
#include "StompFrame.h"
#include <thread>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <sstream>
#include <iostream>
#include <StompFrame.h>
#include <unordered_map>
using std::string;
using std::vector;
using namespace std;

// Splits a string by a delimiter
vector<string> split(string &input, char delimiter) {
    vector<string> lines;
    std::stringstream ss(input);
    string line;
    while (std::getline(ss, line, delimiter)) {
        lines.push_back(line);
    }
    return lines;
}

// Creates a login frame
StompFrame toLoginFrame(const string &host, const string &username, const string &password) {
    string commandLine = "CONNECT";
    unordered_map<string, string> headers = {
        {"accept-version", "1.2"},
        {"host", "stomp.cs.bgu.ac.il"},
        {"login", username},
        {"passcode", password}
    };
    return StompFrame(commandLine, headers, "");
}

int main(int argc, char *argv[]) {
    while (true){ //to enable a client to try login again after fail or after disconnect
       string line;
       getline(cin, line);
       vector<string> args = split(line,' ');
       if(args[0] != "login"){
        cout << "You must login first." << endl;
        continue;
       }
       vector<string> miniArguments = split(args[1], ':');
       string host =  miniArguments[0];
       short port = stoi(miniArguments[1]);
       string username = args[2];
       string password = args[3];
        // Initialize ConnectionHandler
        ConnectionHandler connectionHandler(host, port);
        if (!connectionHandler.connect()) {
            std::cerr << "Could not connect to server" << std::endl;
            continue;
        }
       
        // Send login frame
        StompFrame loginFrame = toLoginFrame(host, username, password);
        string loginFrameStr = loginFrame.toString();
        connectionHandler.sendFrameAscii(loginFrameStr, '\0');

        // Wait for server response
        string serverResponse;
        if (!connectionHandler.getFrameAscii(serverResponse, '\0')) {
            std::cerr << "Failed to receive response from server." << std::endl;
            connectionHandler.close();
            continue;
        }

        StompFrame serverFrame = StompFrame(serverResponse);
        if (serverFrame.getCommand() != "CONNECTED") {
            std::cerr << "Failed to connect: " << connectionHandler.protocol.getLastLine(serverFrame.getBody()) << std::endl;
            connectionHandler.close();
            continue;
        }
        connectionHandler.protocol.connected = true;
        connectionHandler.protocol.userName = username;
        std::cout << "Successfully connected to server." << std::endl;

        // Set up input managers
        KeyboardInputManager readFromKeyboard(connectionHandler);
        ServerInputManager readFromServer(connectionHandler);

        // Start threads
        std::thread thread_readFromServer(&ServerInputManager::run, &readFromServer);
        std::thread thread_readFromKeyboard(&KeyboardInputManager::run, &readFromKeyboard);
        thread_readFromServer.join(); 
        thread_readFromKeyboard.join();
        // Clean up
        connectionHandler.close();
        std::cout << "Socket has been closed and the connection was terminated, if there was an error try to login again" << std::endl;
    }
    return 0;
}
