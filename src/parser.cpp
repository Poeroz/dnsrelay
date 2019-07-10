//
// Created by Poeroz on 2019/7/5.
//

#include "../lib/parser.h"

parser::parser() {
    table = new nameTable();
    sockMan = new socketManager();
    records = new recordTable();
    buffer = new uint8_t[MAX_LEN];

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(UDP_PORT);
    if (!inet_pton(AF_INET, SERVER_DNS_IP, (void *)&(serverAddr.sin_addr))) {
        DBG_ERROR("Cannot convert server's IP to sin_addr.");
    }
}

void parser::receive() {
    sockaddr_in senderAddr;
    bufferSize = sockMan->recvBuffer(buffer, senderAddr, recvTime);
    msg = message(buffer, bufferSize, senderAddr);

    MSGTYPE type = getType();
    if (type == REQUEST) {
        DBG_MESSAGE("DNS-relay receives a query from client.");
        msg.debug();
        parseRequest();
    }
    else if (type == RESPONSE) {
        DBG_MESSAGE("DNS-relay receives a response from server.");
        msg.debug();
        parseResponse();
    }
    else {
        DBG_MESSAGE("DNS-relay receives OTHER message.");
        msg.debug();
        sendToServer();
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
    bool flag = false;
    if (msg.question.size() == 1) {
        auto query = msg.question.front();
        if (query.QTYPE == 1 && query.QCLASS == 1) {
            flag = true;
        }
    }

    if (flag) {
        auto query = msg.question.front();
        std::string name = message::transName(query.QNAME);
        uint32_t IP;
        time_t ddl;
        if (table->query(name, IP, ddl)) {
            addAnswerSection(IP, query.QNAME, ddl);

            DBG_MESSAGE("Successfully found in local table.");
            DBG_MESSAGE("DNS-relay sends the answer to the client.");
            msg.debug();

            uint8_t buffer[MAX_LEN];
            memset(buffer, 0, sizeof(buffer));
            int bufferSize = 0;
            msg.struct2Buffer(buffer, bufferSize);

            sockMan->sendBuffer(buffer, bufferSize, msg.senderAddr);
        }
        else {
            DBG_MESSAGE("Not found locally, send to the server...");
            sendToServer();
        }
    }
    else {
        DBG_MESSAGE("DNS-relay can not handle this message, send to the server...");
        sendToServer();
    }
}

void parser::addAnswerSection(uint32_t IP, std::string name, time_t ddl) {
    if (IP == (uint32_t)0) {
        msg.header.QR = true;
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
        tmpAnswer.TTL = ddl - std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
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
    DBG_MESSAGE("ID transfer : " + std::to_string(ntohs(*IDptr)) + " -> " + std::to_string(newID));
    *IDptr = htons(newID);
    sockMan->sendBuffer(buffer, bufferSize, serverAddr);
}

void parser::parseResponse() {
    recordTable::record tmpRecord;
    if (!records->findRecord(msg.header.ID, tmpRecord)) {
        DBG_ERROR("Record not found.");
    }
    else {
        auto IDptr = (uint16_t *)buffer;
        DBG_MESSAGE("ID transfer : " + std::to_string(ntohs(*IDptr)) + " -> " + std::to_string(tmpRecord.id));
        *IDptr = htons(tmpRecord.id);
        msg.buffer2Struct(buffer);

        DBG_MESSAGE("DNS-relay sends the answer to the client.");
        msg.debug();

        for (auto ans : msg.RR[message::ANSWER]) {
            if (ans.TYPE == 1 && ans.CLASS == 1) {
                uint32_t IP = *(uint32_t *)ans.RDATA.data();
                time_t ddl = recvTime + ans.TTL;
                table->insertItem(message::transName(msg.question.front().QNAME), IP, ddl);
                break;
            }
        }

        sockMan->sendBuffer(buffer, bufferSize, tmpRecord.senderAddr);
    }
}