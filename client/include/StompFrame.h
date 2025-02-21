#ifndef STOMPFRAME_H
#define STOMPFRAME_H

#include <string>
#include <unordered_map>

class StompFrame {
private:
    std::string command;                               
    std::unordered_map<std::string, std::string> headers; 
    std::string body;                                

public:
    // Constructors
    StompFrame(const std::string &command, 
               std::unordered_map<std::string, std::string> headers, 
               const std::string &body);

    StompFrame(const std::string &rawFrame); 


    std::string toString() const;

    // Accessor methods
    const std::string& getCommand() const;                          
    const std::string& getHeader(const std::string& key) const;     
    const std::string& getBody() const;  
    const std::unordered_map<std::string, std::string> getHeaders() const;                      
};

#endif // STOMPFRAME_H
