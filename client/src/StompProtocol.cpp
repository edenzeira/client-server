#include "StompFrame.h"
#include "StompProtocol.h"
#include "../include/ConnectionHandler.h"
#include "StompEMIClient.h"
#include "event.h"
#include <fstream>
#include <algorithm>
#include <cctype>
#include <locale>
using namespace std;

StompProtocol::StompProtocol(): topics({}),  receiptGenerator(0), receiptType(""), subIdsGenerator(0), events(), frameList(), userName(""), mtx(), connected(false), shouldTerminate(false){}

list<StompFrame> StompProtocol::handle_login (std::vector<std::string> lines){
    string commandLine = "CONNECT";
    unordered_map<string,string> headers;
    headers["accept-version"] = "1.2";
    headers["host"] = "stomp.cs.bgu.ac.il";
    headers["login"] = lines[2];
    headers["passcode"] = lines[3];
    string body = "";
    userName = lines[2];
    StompFrame frame = StompFrame(commandLine, headers, body);
    frameList.push_back(frame);
    return frameList;
}

list<StompFrame> StompProtocol::handle_logout (std::vector<std::string> lines){
    string commandLine = "DISCONNECT";
    unordered_map<string,string> headers;
    receiptGenerator++;
    headers["receipt"] = to_string(receiptGenerator);
    receiptType = "logout";
    string body = "";
    StompFrame frame = StompFrame(commandLine, headers, body);
    frameList.push_back(frame);
    return frameList;
}

list<StompFrame> StompProtocol::handle_join (std::vector<std::string> lines){
    string commandLine = "SUBSCRIBE";
    unordered_map<string,string> headers;
    headers["destination"] = "/" + lines[1];
    subIdsGenerator++;
    headers["id"] = std::to_string(subIdsGenerator);
    receiptGenerator++;
    headers["receipt"] = to_string(receiptGenerator);
    receiptType = "subscribe " + lines[1] + " " + to_string(subIdsGenerator);
    string body = "";
    StompFrame frame = StompFrame(commandLine, headers, body);
    frameList.push_back(frame);
    return frameList;
}

list<StompFrame> StompProtocol::handle_exit (std::vector<std::string> lines){
    string commandLine = "UNSUBSCRIBE";
    unordered_map<string,string> headers;
    headers["id"] = to_string(topics[lines[1]]) ;
    receiptGenerator++;
    headers["receipt"] = to_string(receiptGenerator);
    receiptType = "unsubscribe " + lines[1] + " " + to_string(topics[lines[1]]);
    string body = "";
    StompFrame frame = StompFrame(commandLine, headers, body);
    frameList.push_back(frame);
    return frameList;
}

list<StompFrame> StompProtocol::handle_report (std::vector<std::string> lines){
    string commandLine = "SEND";
    unordered_map<string,string> headers;
    names_and_events nameEvents = parseEventsFile(lines[1]); 
    headers["destination"] = "/" + nameEvents.channel_name;
    for (Event e : nameEvents.events){
        e.setEventOwnerUser(userName);
        string body = e.toString() + "\n";
        StompFrame frame(commandLine, headers, body);
        events[nameEvents.channel_name][userName].push_back(e); //add the event to the events
        frameList.push_back(frame);
        receiptType = "send";
    }
    return frameList;
}

list<StompFrame> StompProtocol::process(string &convert){ //process the input from the keyboard
    frameList.clear();
    std::vector<std::string> lines = split(convert);
    string type = lines[0];
    
    //login - CONNECT
    if(type == "login"){
        return handle_login(lines);
    }
    //logout - DISCONNECT
    else if(type == "logout"){
        return handle_logout(lines);
    }
    //join - SUBSCRIBE
    else if(type == "join"){
        return handle_join(lines);
    }
    //exit - UNSUBSCRIBE
    else if(type == "exit"){
        return handle_exit(lines);
    }
    //report - SEND
    else { //type == "report"
        return handle_report(lines); 
    }  
}

void StompProtocol::receiveProcess(StompFrame &frame){
    string commandLine = frame.getCommand();
    //CONNECTED
    if(commandLine == "CONNECTED"){
        connected = true;
        cout << "Login successful" << endl;
    }
    
    //MESSAGE
    if(commandLine == "MESSAGE"){
        string topic = frame.getHeader("destination").substr(1);
        //extract the user name
        istringstream stream(frame.getBody());
        string firstLine;
        getline(stream, firstLine);
        string user = firstLine.substr(6);

        Event event = Event(frame.toString());

        if(events.count(topic)){ //check if contains
            if (events[topic].count(user))
                events[topic][user].push_back(event);
            else{
                list<Event> list = {};
                list.push_back(event);
                events[topic][user] = list;
            }
        }
        else {
            unordered_map<string , list<Event>> map = {};
            list<Event> list = {};
            list.push_back(event);
            map[user] = list;
            events[topic] = map;
        }
        
        cout << frame.getBody() << endl; 
    }
    //RECEIPT
    if(commandLine == "RECEIPT"){
        string receiptNum = frame.getHeader("receipt-id");
        if(receiptNum == to_string(receiptGenerator)) { 

            vector<string> v = split(receiptType);

            if(v[0] == "logout"){ //receiptType structure : "logout"
                topics.clear();
                connected = false;
                shouldTerminate = true;
            }
            else if(v[0] == "subscribe"){ //receiptType structure : "subscribed topic subId"
                topics[v[1]] = stoi(v[2]); //add subscribe to the topics map
                cout << "Joined channel " + v[1] << endl;
            }
            else if(v[0] == "unsubscribe"){ //receiptType structure : "unsubscribed topic subId"
                topics.erase(v[1]);
                cout << "Exited channel " + v[1] << endl;
            }
        } 
    }

     //ERROR 
    if(commandLine == "ERROR"){
        connected = false;
        shouldTerminate = true;
        cout << getLastLine(frame.getBody()) << endl; //print the body of the error
    }
    
}


std::string StompProtocol::getLastLine(const std::string &input) {
    std::istringstream stream(input);
    std::string line, lastLine;

    // Read each line from the string
    while (std::getline(stream, line)) {
        lastLine = line; // Update lastLine with the current line
    }

    return lastLine; // Return the last line
}

//aid function
vector<string> StompProtocol::split(const string &input) {
    vector<string> result;
    istringstream stream(input);
    string token;

    while (getline(stream, token, ' ')) {
        result.push_back(token);
    }

    return result;
}

void StompProtocol::summaryProcess(vector<string> args){
    string channelName = args[1];
    string sender = args[2];
    int reportsCounter = 0;
    int activeCounter = 0;
    int forcesCounter = 0;
    ostringstream summary;
    vector<Event> eventVector;

    // Check if there are events
    if (events.empty()) {
        summary << "No events available for this channel";
    } 
    else {
        // Iterate over the events and format each one
        for (const auto &ev : events) { //loop over topics
            if(ev.first == channelName){ //find the right topic
                for (const auto &eventMap : ev.second) { //loop over usernames
                    if(eventMap.first == sender){ //find the right username
                        for(const auto& event: eventMap.second){ //save all events that this username has sent
                            reportsCounter++;
                            eventVector.push_back(event);
                            map<string,string> general = event.get_general_information();
                            if(userName != sender  && general["active"].substr(1,4) == "true")
                                activeCounter++;
                            else if (userName == sender  && general["active"] == "true")
                                activeCounter++;
                            if(userName != sender  && general["forces_arrival_at_scene"].substr(1,4) == "true")
                                forcesCounter++;
                            else if (userName == sender  && general["forces_arrival_at_scene"] == "true")
                                forcesCounter++;
                        }
                    }
                }
            }
        }

        if (!eventVector.empty()){
            sort(eventVector.begin() , eventVector.end() , [](const Event &a , const Event &b){
                if (a.get_date_time() == b.get_date_time()){
                    return a.get_name() < b.get_name();  // Sort by name if date_time is equal
                }
                return a.get_date_time() < b.get_date_time(); // Sort by date_time otherwise
            });

            summary << "Channel " << channelName << "\n"; 
            summary << "Stats:" << "\n";
            summary << "Total: " << to_string(reportsCounter) << "\n";
            summary << "active: " << to_string(activeCounter) << "\n";
            summary << "forces arraival at scene: " << to_string(forcesCounter) << "\n" << "\n";
            summary << "Event Reports:" << "\n" << "\n";

            int reportsCounter2 = 0;
            for(Event e : eventVector){
                reportsCounter2++;
                summary << "Report_" << to_string(reportsCounter2) << ":" << "\n";
                summary <<e.summeryString();
            } 
        }

        //no relevant events 
        else{
            summary << "No events available for this channel";
        }
    }

    // Write the summary to the specified file
    std::ofstream outFile(args[3]); 
    //open a new file if he is doesnt exist
    //if the file name is just a file he will be created in the current working directory of the program.
    //if the file name is a full path he will be created at the specified location.
    if (outFile.is_open()) {
        outFile << summary.str(); //if the file is not empty he will overwirte it
        outFile.close();
        std::cout << "Summary written to file: " << args[3] << std::endl;
    } else {
        std::cerr << "Error: Unable to open file " << args[3] << std::endl;
    }
}