//
// Created by Poeroz on 2019/7/6.
//

#include "recordTable.h"

recordTable::recordTable() {
    currentID = 0;
}

uint16_t recordTable::insertRecord(message msg) {
    record tmpRecord;
    tmpRecord.id = msg.header.ID;
    tmpRecord.senderAddr = msg.senderAddr;
    tmpRecord.arriveTime = std::chrono::system_clock::now();
    table.push_back(std::make_pair(++currentID, tmpRecord));
    return currentID;
}

bool recordTable::findRecord(uint16_t id, recordTable::record &tmpRecord) {
    for (auto it = table.begin(); it != table.end(); it++) {
        if (it->first == id) {
           tmpRecord = it->second;
           table.erase(it);
           return true;
        }
    }
    return false;
}

void recordTable::deleteTimeoutRecord() {
    for (auto it = table.begin(); it != table.end();) {
        if (std::chrono::system_clock::now() - it->second.arriveTime > timeout) {
            it = table.erase(it);
        }
        else {
            it++;
        }
    }
}
