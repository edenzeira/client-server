
#include "ServerInputManager.h"
#include "StompFrame.h"
#include <map>
#include <string>
using std::map;
using std::string;
using std::pair;





ServerInputManager::ServerInputManager(ConnectionHandler &connectionHandler): connectionHandler(connectionHandler){};

void ServerInputManager::run(){
    while(!connectionHandler.protocol.shouldTerminate && connectionHandler.protocol.connected){
        string serverInput = "";
        connectionHandler.protocol.mtx.lock();
        if(connectionHandler.getFrame(serverInput) == false){
            connectionHandler.protocol.shouldTerminate = true;
            break;
        }
        //Send to protocol to manage the input
      StompFrame frame(serverInput);
      connectionHandler.protocol.receiveProcess(frame);
      connectionHandler.protocol.mtx.unlock();
    }
    }