#pragma once

#include "event.h"
#include "StompFrame.h"
#include <map>
#include <mutex>
#include <unordered_map>
#include <string>
#include <list>
using namespace std;
using std::unordered_map;
using std::list;


class StompProtocol
{
private:
    unordered_map<string, int> topics; //topicPerSubId
    int receiptGenerator;
    string receiptType; //the type of the receipt and details
    int subIdsGenerator;
    unordered_map<string, unordered_map<string, list<Event>>> events; //topicPerMap<user, list of events>
    list<StompFrame> frameList;

    
    
public:
    
    list<StompFrame> process(string &convert);
    list<StompFrame> handle_login (std::vector<std::string> lines);
    list<StompFrame> handle_logout (std::vector<std::string> lines);
    list<StompFrame> handle_join (std::vector<std::string> lines);
    list<StompFrame> handle_exit (std::vector<std::string> lines);
    list<StompFrame> handle_report (std::vector<std::string> lines);
    void summaryProcess(vector<string> args);
    std::string getLastLine(const std::string &input);
    string userName;
    std::mutex mtx;
    bool connected;
    bool shouldTerminate;
    StompProtocol();
    void receiveProcess(StompFrame &frame);

    vector<string> split(const string &input);

};