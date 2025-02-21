#include "KeyboardInputManager.h"
#include "StompFrame.h"
#include "event.h"
#include <string>
#include <map>
#include <list>
#include <vector>
#include <json.hpp>
#include <fstream>
#include <mutex>
using std::unordered_map;
using std::string;
using std::pair;
#include <iostream>
#include <string>
using json = nlohmann::json;
using namespace std;




KeyboardInputManager::KeyboardInputManager(ConnectionHandler &connectionHandler): connectionHandler(connectionHandler),receiptNum(0){};

void KeyboardInputManager::run(){
    string keyboardInput = "";
    while(!connectionHandler.protocol.shouldTerminate){
        //Read from keyboard and insert to keyboardInput
        getline(std::cin, keyboardInput);
        //send to protocol to handle the input
        if(!connectionHandler.protocol.shouldTerminate){
            vector<string> args = connectionHandler.protocol.split(keyboardInput);
            if(args[0] == "summary"){
                connectionHandler.protocol.summaryProcess(args);
            } 
            else {
                list<StompFrame> frameList = connectionHandler.protocol.process(keyboardInput);
                for(auto& frame: frameList){
                    if(args[0] == "login" &&  connectionHandler.protocol.connected){
                        std::cout << "The client is already logged in, log out before trying again" << std::endl;
                    } else {
                        string output = frame.toString();
                        if(connectionHandler.sendFrame(output) == false){
                            connectionHandler.protocol.shouldTerminate = true;
                            break;
                        }
                        
                        if(args[0] == "logout"){
                           connectionHandler.protocol.mtx.lock();
                           connectionHandler.protocol.mtx.unlock();
                        }
                    }
                }
            }
        }
    }
}

