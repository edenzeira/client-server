#pragma once
#include "ConnectionHandler.h"
#include <list>


class KeyboardInputManager
{
private:
    ConnectionHandler &connectionHandler;
    int receiptNum;

    
public:
    KeyboardInputManager(ConnectionHandler &connectionHandler);
    void run();
  
};