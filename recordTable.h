//
// Created by Poeroz on 2019/7/6.
//

#ifndef DNSRELAY_RECORDTABLE_H
#define DNSRELAY_RECORDTABLE_H

#include <map>
#include <cstdint>
#include <string>
#include <chrono>
#include "message.h"
#include "socketHeader.h"

const std::chrono::seconds timeout = std::chrono::seconds(10);

class recordTable {
public:
    struct record {
        uint16_t id;
        sockaddr_in senderAddr;
        std::chrono::system_clock::time_point arriveTime;
    };

    recordTable();


    uint16_t insertRecord(message msg);
    bool findRecord(uint16_t id, record &tmpRecord);
    void deleteTimeoutRecord();

private:
    uint16_t currentID;
    std::vector<std::pair<uint16_t, recordTable::record> > table;
};


#endif //DNSRELAY_RECORDTABLE_H
