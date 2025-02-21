#include "StompFrame.h"
#include <sstream>
#include <stdexcept>
#include <string>
using namespace std;

StompFrame::StompFrame(const string &command, unordered_map<string, string> headers, const string &body): command(command), headers(headers), body(body){}


string StompFrame::toString() const {
    ostringstream oss;
    oss << command << "\n";
    for (const auto& header : headers) {
        oss << header.first << ":" << header.second << "\n";
    }
    oss << "\n" << body; 
    return oss.str();
}

//construct a StompFrame from a string
StompFrame::StompFrame(const string &rawFrame):command(), headers(), body() {
    istringstream iss(rawFrame);
    string line;

    // Parse command (first line)
    getline(iss, this->command);

    // Parse headers
    while (getline(iss, line) && !line.empty()) {
        size_t colonPos = line.find(':');
        if (colonPos != string::npos) { // Ensure the colon exists
            string key = line.substr(0, colonPos);
            string value = line.substr(colonPos + 1);
            this->headers[key] = value;
        }
    }

    // Parse body (remaining content after the blank line)
    this->body = string((istreambuf_iterator<char>(iss)), istreambuf_iterator<char>());
}



const string& StompFrame::getCommand() const {
    return command;
}

const string& StompFrame::getHeader(const string& key) const {
    return headers.at(key);
}

const unordered_map<string,string> StompFrame::getHeaders() const {
    return headers;
}

const string& StompFrame::getBody() const {
    return body;
}
