//
// Created by Poeroz on 2019/7/4.
//

#include "../lib/nameTable.h"

nameTable::nameTable(){
    domainMap.clear();
    ipSet.clear();
    ddlSet.clear();
    read_hostfile();
    read_cachefile();
}

void nameTable::read_hostfile() {
    std::ifstream fin(HOST_FILE);
    if (fin.is_open()) {
        std::string IPstr, name;
        while (fin >> IPstr >> name) {
            uint32_t IP;
            in_addr tmpAddr;
            inet_pton(AF_INET, IPstr.c_str(), (void *) &tmpAddr);
            IP = tmpAddr.s_addr;

            nameItem tmp;
            tmp.name = name;
            tmp.IP = IP;
            tmp.ddl = LONG_MAX;

            domainMap.insert(std::make_pair(tmp.name, tmp));
            ddlSet.insert(tmp);
            ipSet.insert(tmp);
        }
    }
}

void nameTable::read_cachefile() {
    std::ifstream fin(CACHE_FILE);
    if (fin.is_open()) {
        std::string IPstr, name;
        time_t ddl;
        while (fin >> IPstr >> name >> ddl) {
            uint32_t IP;
            in_addr tmpAddr;
            inet_pton(AF_INET, IPstr.c_str(), (void *)&tmpAddr);
            IP = tmpAddr.s_addr;

            nameItem tmp;
            tmp.name = name;
            tmp.IP = IP;
            tmp.ddl = ddl;

            domainMap.insert(std::make_pair(tmp.name, tmp));
            ddlSet.insert(tmp);
            ipSet.insert(tmp);
        }
    }
    clearTimeoutItem();
}

bool nameTable::query(std::string name, uint32_t &IP, time_t &ddl) {
    clearTimeoutItem();
    if (domainMap.find(name) == domainMap.end()) {
        return false;
    }
    else {
        nameItem tmp = domainMap[name];
        IP = tmp.IP;
        ddl = tmp.ddl;
        return true;
    }
}

void nameTable::insertItem(std::string name, uint32_t IP, time_t ddl) {
    nameItem tmp;
    tmp.name = name;
    tmp.IP = IP;
    tmp.ddl = ddl;

    domainMap.insert(std::make_pair(name, tmp));
    ddlSet.insert(tmp);
    ipSet.insert(tmp);

    write_file();
}

void nameTable::clearTimeoutItem() {
    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    for (auto iter = ddlSet.begin(); iter != ddlSet.end();) {
        if (iter->ddl <= now) {
            domainMap.erase(domainMap.find(iter->name));
            ipSet.erase(ipSet.find(*iter));
            iter = ddlSet.erase(iter);
        }
        else {
            iter++;
        }
    }
    write_file();
}

void nameTable::write_file() {
    std::ofstream fout(CACHE_FILE);
    if (fout.is_open()) {
        for (auto it : ipSet) {
            if (it.ddl == LONG_MAX) continue;
            uint32_t IP = it.IP;
            char str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, (void *)&IP, str, INET_ADDRSTRLEN);
            fout << str << ' ' << it.name << ' ' << it.ddl << std::endl;
        }
    }
}