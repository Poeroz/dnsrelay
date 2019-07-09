//
// Created by Poeroz on 2019/7/5.
//

#ifndef DNSRELAY_PARSER_H
#define DNSRELAY_PARSER_H

#include "nameTable.h"
#include "socketManager.h"
#include "recordTable.h"
#include <string>

class parser {
public:
    parser();
    void receive();

    enum MSGTYPE {
        REQUEST, RESPONSE, OTHER
    };

private:
    uint8_t *buffer;
    int bufferSize;
    time_t recvTime;
    message msg;
    sockaddr_in serverAddr;

    recordTable *records;
    nameTable *table;
    socketManager *sockMan;

    parser::MSGTYPE getType();

    void parseRequest();
    void parseResponse();
    void sendToServer();
    void addAnswerSection(uint32_t IP, std::string name, time_t ddl);
};


#endif //DNSRELAY_PARSER_H
