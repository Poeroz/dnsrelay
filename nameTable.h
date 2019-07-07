//
// Created by Poeroz on 2019/7/4.
//

#ifndef DNSRELAY_NAMETABLE_H
#define DNSRELAY_NAMETABLE_H

#define FILE_PATH "/Users/poeroz/Downloads/dnsrelay/dnsrelay.txt"
#define DBG_PATH "/Users/poeroz/Downloads/dnsrelay/debug.txt"

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "socketHeader.h"

class nameTable {
public:
    nameTable();
    void initTable();
    bool query(std::string name, uint32_t &IP);
private:
    std::map<std::string, uint32_t> nameToIP;
};


#endif //DNSRELAY_NAMETABLE_H
