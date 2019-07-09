//
// Created by Poeroz on 2019/7/4.
//

#ifndef DNSRELAY_NAMETABLE_H
#define DNSRELAY_NAMETABLE_H

#define HOST_FILE "/Users/poeroz/Downloads/dnsrelay/file/dnsrelay.txt"
#define CACHE_FILE "/Users/poeroz/Downloads/dnsrelay/file/cache.txt"
#define DBG_ERROR(m) std::cout << "ERROR : " << (m) << std::endl

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include "socketHeader.h"

class nameTable {
public:
    struct nameItem {
        std::string name;
        uint32_t IP;
        time_t ddl;
    };

    struct ddlcmp {
        bool operator()(nameTable::nameItem lhs, nameTable::nameItem rhs) {
            return lhs.ddl < rhs.ddl;
        }
    };

    struct ipcmp {
        bool operator()(nameTable::nameItem lhs, nameTable::nameItem rhs) {
            return lhs.IP < rhs.IP;
        }
    };

    nameTable();
    void read_hostfile();
    void read_cachefile();
    void clearTimeoutItem();
    void insertItem(std::string name, uint32_t IP, time_t ddl);
    bool query(std::string name, uint32_t &IP, time_t &ddl);
private:
    std::map<std::string, nameTable::nameItem> domainMap;
    std::multiset<nameTable::nameItem, nameTable::ddlcmp> ddlSet;
    std::multiset<nameTable::nameItem, nameTable::ipcmp> ipSet;
    void write_file();
};


#endif //DNSRELAY_NAMETABLE_H
