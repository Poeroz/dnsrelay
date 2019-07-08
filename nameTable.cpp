//
// Created by Poeroz on 2019/7/4.
//

#include "nameTable.h"

nameTable::nameTable(){
    nameToIP.clear();
    initTable();
}

void nameTable::initTable() {
    std::ifstream fin(FILE_PATH);
    if (fin.is_open()) {
        std::string IPstr, name;
        while (fin >> IPstr >> name) {
            uint32_t IP;
            in_addr tmpAddr;
            inet_pton(AF_INET, IPstr.c_str(), (void *)&tmpAddr);
            IP = tmpAddr.s_addr;
            nameToIP.insert(std::make_pair(name, IP));
        }
    }
    else {

    }
}

bool nameTable::query(std::string name, uint32_t &IP) {
    if (nameToIP.find(name) == nameToIP.end()) {
        return false;
    }
    else {
        IP = nameToIP[name];
        return true;
    }
}