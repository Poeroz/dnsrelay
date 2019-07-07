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
//    std::ofstream fout(DBG_PATH);
    if (fin.is_open()) {
        std::string IPstr, name;
        while (fin >> IPstr >> name) {
            uint32_t IP;
            in_addr tmpAddr;
            inet_pton(AF_INET, IPstr.c_str(), (void *)&tmpAddr);
            IP = tmpAddr.s_addr;
            if (nameToIP.find(name) != nameToIP.end()) {
                std::cout << name << " has exist!" << std::endl;
            }
            else {
                nameToIP.insert(std::make_pair(name, IP));
            }
        }
    }
    else {
        std::cout << "file not open!" << std::endl;
    }
//    for (auto pss : nameToIP) {
//        fout << pss.first << ' ' << pss.second << std::endl;
//    }
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