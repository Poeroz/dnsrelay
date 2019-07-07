//
// Created by Poeroz on 2019/7/5.
//

#include "parser.h"
#include "recordTable.h"

parser::parser() {
    table = new nameTable();
    sockMan = new socketManager();
    records = new recordTable();
    buffer = new uint8_t[MAX_LEN];

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(UDP_PORT);
    if (!inet_pton(AF_INET, SERVER_DNS_IP, (void *)&(serverAddr.sin_addr))) {
        ERR_EXIT("server dns IP error");
    }
}

void parser::receive() {
    sockaddr_in senderAddr;
    bufferSize = sockMan->recvBuffer(buffer, senderAddr, recvTime);
    msg = message(buffer, bufferSize, senderAddr);

//    std::cout << "receive address" << std::endl;
//    std::cout << senderAddr.sin_addr.s_addr << std::endl;
//    std::cout << senderAddr.sin_port << std::endl;

    std::cout << "receive message : " << std::endl;
    msg.debug();

    MSGTYPE type = getType();
    if (type == REQUEST) {
        parseRequest();
    }
    else if (type == RESPONSE) {
        parseResponse();
    }
    else {

    }
}

parser::MSGTYPE parser::getType() {
    if (msg.header.QR == 0 && msg.header.OPCODE == 0) {
        return REQUEST;
    }
    else if (msg.header.QR == 1) {
        return RESPONSE;
    }
    else {
        return OTHER;
    }
}

void parser::parseRequest() {
    bool flag = true;
    for (auto query : msg.question) {
        if (query.QTYPE == 1 && query.QCLASS == 1) {
            std::string name = message::transName(query.QNAME);
            uint32_t IP;
            if (table->query(name, IP)) {
                std::cout << "Successfully founded in local table." << std::endl;
                addAnswerSection(IP, query.QNAME);
            }
            else {
                std::cout << "Not founded locally, to the server..." << std::endl;
                sendToServer();
                flag = false;
                break;
            }
        }
    }
    if (flag) {
        uint8_t buffer[MAX_LEN];
        memset(buffer, 0, sizeof(buffer));
        int bufferSize = 0;
        msg.struct2Buffer(buffer, bufferSize);
        msg.debug();

        sockMan->sendBuffer(buffer, bufferSize, msg.senderAddr);
    }
}

void parser::addAnswerSection(uint32_t IP, std::string name) {
    if (IP == (uint32_t)0) {
        msg.header.RCODE = 3;
    }
    else {
        msg.header.QR = true;
        msg.header.AA = true;
        msg.header.RA = true;
        msg.header.RRCOUNT[message::ANSWER]++;
        message::RESOURCE_RECORD tmpAnswer;
        tmpAnswer.NAME = name;
        tmpAnswer.TYPE = 1;
        tmpAnswer.CLASS = 1;
        tmpAnswer.RDLENGTH = 4;
        tmpAnswer.RDATA.assign((uint8_t *)&IP, (uint8_t *)(&IP + 1));
        msg.RR[message::ANSWER].push_back(tmpAnswer);
    }
}

void parser::sendToServer() {
    records->deleteTimeoutRecord();
    auto newID = records->insertRecord(msg);
    auto IDptr = (uint16_t *)buffer;
    *IDptr = htons(newID);
    sockMan->sendBuffer(buffer, bufferSize, serverAddr);
}

void parser::parseResponse() {
    recordTable::record tmpRecord;
    if (!records->findRecord(msg.header.ID, tmpRecord)) {
        ERR_EXIT("record error");
    }
    else {
        auto IDptr = (uint16_t *)buffer;
        *IDptr = htons(tmpRecord.id);
        sockMan->sendBuffer(buffer, bufferSize, tmpRecord.senderAddr);
    }
}