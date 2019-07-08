//
// Created by Poeroz on 2019/7/5.
//

#include "message.h"

message::message(uint8_t *buffer, int bufferSize, sockaddr_in senderAddr) {
    this->senderAddr = senderAddr;
    startPos = (uint8_t *)buffer;
    question.clear();
    for (int i = 0; i < 3; i++) {
        RR[i].clear();
    }

    buffer2Struct((uint8_t *) buffer);
}

void message::buffer2Struct(uint8_t *ptr) {
    buffer2Header(ptr);
    buffer2Question(ptr);
    buffer2RR(ptr, ANSWER);
    buffer2RR(ptr, AUTHORITY);
    buffer2RR(ptr, ADDTIONAL);
}

void message::buffer2Header(uint8_t *&ptr) {
    uint16_t tmp;

    getUint16(header.ID, ptr);
    getUint16(tmp, ptr);

    header.QR       = (bool)        ((tmp & 0x8000) >> 15);
    header.OPCODE   = (uint8_t)     ((tmp & 0x7800) >> 11);
    header.AA       = (bool)        ((tmp & 0x0400) >> 10);
    header.TC       = (bool)        ((tmp & 0x0200) >> 9);
    header.RD       = (bool)        ((tmp & 0x0100) >> 8);
    header.RA       = (bool)        ((tmp & 0x0080) >> 7);
    header.ZERO     = (uint8_t)     ((tmp & 0x0070) >> 4);
    header.RCODE    = (uint8_t)     ((tmp & 0x000F));

    getUint16(header.QDCOUNT, ptr);
    for (unsigned short &i : header.RRCOUNT) {
        getUint16(i, ptr);
    }
}

void message::buffer2Question(uint8_t *&ptr) {
    for (int i = 0; i < header.QDCOUNT; i++) {
        QUESTION tmpQuestion;
        std::string name = "";
        unpackName(ptr, name);
        tmpQuestion.QNAME = name;
        getUint16(tmpQuestion.QTYPE, ptr);
        getUint16(tmpQuestion.QCLASS, ptr);
        question.push_back(tmpQuestion);
    }
}

void message::buffer2RR(uint8_t *&ptr, message::RRTYPE type) {
    for (int i = 0; i < header.RRCOUNT[type]; i++) {
        RESOURCE_RECORD tmpRR;
        std::string name = "";
        unpackName(ptr, name);
        tmpRR.NAME = name;
        getUint16(tmpRR.TYPE, ptr);
        getUint16(tmpRR.CLASS, ptr);
        getUint32(tmpRR.TTL, ptr);
        getUint16(tmpRR.RDLENGTH, ptr);
        tmpRR.RDATA.assign(ptr, ptr + tmpRR.RDLENGTH);
        ptr += tmpRR.RDLENGTH;
        RR[type].push_back(tmpRR);
    }
}

void message::unpackName(uint8_t *&ptr, std::string &name) {

    while (*ptr != 0) {
        if ((*ptr & 0xc0) == 0xc0) {
            uint16_t offset;
            getUint16(offset, ptr);
            uint8_t *next = startPos + (offset & 0x3fff);
            unpackName(next, name);
            return;
        }
        else {
            uint8_t len = *ptr;
            name.append((char *)ptr, len + 1);
            ptr += len + 1;
        }
    }
    name.append(1, '\0');
    ptr++;
}

void message::getUint32(uint32_t &var, uint8_t *&ptr) {
    var = ntohl(*(uint32_t *)ptr);
    ptr += sizeof(uint32_t);
}

void message::getUint16(uint16_t &var, uint8_t *&ptr) {
    var = ntohs(*(uint16_t *)ptr);
    ptr += sizeof(uint16_t);
}

void message::putUint16(uint16_t var, uint8_t *&ptr, int &bufferSize) {
    auto intPtr = (uint16_t *)ptr;
    *intPtr = (uint16_t)(htons(var));
    bufferSize += sizeof(uint16_t);
    ptr += sizeof(uint16_t);
}

void message::putUint32(uint32_t var, uint8_t *&ptr, int &bufferSize) {
    auto intptr = (uint32_t *)ptr;
    *intptr = (uint32_t)(htonl(var));
    bufferSize += sizeof(uint32_t);
    ptr += sizeof(uint32_t);
}

std::string message::transName(std::string name) {
    std::string ans = "";
    const char *ptr = name.c_str();
    while (*ptr != '\0') {
        int size = *ptr;
        ptr++;
        ans.append(ptr, size);
        ptr += size;
        if (*ptr != '\0') {
            ans.append(1, '.');
        }
    }
    return ans;
}

void message::struct2Buffer(uint8_t *ptr, int &bufferSize) {
    header2Buffer(ptr, bufferSize);
    question2Buffer(ptr, bufferSize);
    RR2Buffer(ptr, bufferSize, ANSWER);
    RR2Buffer(ptr, bufferSize, AUTHORITY);
    RR2Buffer(ptr, bufferSize, ADDTIONAL);
}

void message::header2Buffer(uint8_t *&ptr, int &bufferSize) {
    uint16_t tmp = 0;
    tmp |= header.QR        << 15;
    tmp |= header.OPCODE    << 11;
    tmp |= header.AA        << 10;
    tmp |= header.TC        << 9;
    tmp |= header.RD        << 8;
    tmp |= header.RA        << 7;
    tmp |= header.ZERO      << 4;
    tmp |= header.RCODE;

    putUint16(header.ID, ptr, bufferSize);
    putUint16(tmp, ptr, bufferSize);
    putUint16(header.QDCOUNT, ptr, bufferSize);
    for (unsigned short i : header.RRCOUNT) {
        putUint16(i, ptr, bufferSize);
    }
}

void message::question2Buffer(uint8_t *&ptr, int &bufferSize) {
    for (auto query : question) {
        memcpy((void *)ptr, (void *)query.QNAME.c_str(), query.QNAME.size());
        bufferSize += query.QNAME.size();
        ptr += query.QNAME.size();
        putUint16(query.QTYPE, ptr, bufferSize);
        putUint16(query.QCLASS, ptr, bufferSize);
    }
}

void message::RR2Buffer(uint8_t *&ptr, int &bufferSize, message::RRTYPE type) {
    for (auto rr : RR[type]) {
        memcpy((void *)ptr, (void *)rr.NAME.c_str(), rr.NAME.size());
        bufferSize += rr.NAME.size();
        ptr += rr.NAME.size();

        putUint16(rr.TYPE, ptr, bufferSize);
        putUint16(rr.CLASS, ptr, bufferSize);
        putUint32(rr.TTL, ptr, bufferSize);
        putUint16(rr.RDLENGTH, ptr, bufferSize);

        memcpy((void *)ptr, (void *)rr.RDATA.data(), rr.RDATA.size());
        bufferSize += rr.RDATA.size();
        ptr += rr.RDATA.size();
    }
}

void message::debug() {
    std::cout << "------------------Header------------------" << std::endl;
    std::cout << "ID        : " << header.ID << std::endl;
    std::cout << "QR        : " << header.QR << std::endl;
    std::cout << "Opcode    : " << (unsigned)header.OPCODE << std::endl;
    std::cout << "AA        : " << header.AA << std::endl;
    std::cout << "TC        : " << header.TC << std::endl;
    std::cout << "RD        : " << header.RD << std::endl;
    std::cout << "RA        : " << header.RD << std::endl;
    std::cout << "RCODE     : " << (unsigned)header.RCODE << std::endl;
    std::cout << "QDCOUNT   : " << header.QDCOUNT << std::endl;
    std::cout << "ANCOUNT   : " << header.RRCOUNT[message::ANSWER] << std::endl;
    std::cout << "NSCOUNT   : " << header.RRCOUNT[message::AUTHORITY] << std::endl;
    std::cout << "ARCOUNT   : " << header.RRCOUNT[message::ADDTIONAL] << std::endl;

    std::cout << "-----------------Question-----------------" << std::endl;
    for (int i = 0; i < question.size(); i++) {
        std::cout << "[question " << i << "]" << std::endl;
        std::cout << "QNAME     : ";
        debug_str(question[i].QNAME);
        std::cout << "QTYPE     : " << question[i].QTYPE << std::endl;
        std::cout << "QCLASS    : " << question[i].QCLASS << std::endl;
    }

    std::cout << "------------------Answer------------------" << std::endl;
    for (int i = 0; i < RR[message::ANSWER].size(); i++) {
        std::cout << "[answer " << i << "]" << std::endl;
        std::cout << "NAME      : ";
        debug_str(RR[message::ANSWER][i].NAME);
        std::cout << "TYPE      : " << RR[message::ANSWER][i].TYPE << std::endl;
        std::cout << "CLASS     : " << RR[message::ANSWER][i].CLASS << std::endl;
        std::cout << "TTL       : " << RR[message::ANSWER][i].TTL << std::endl;
        std::cout << "RDLENGTH  : " << RR[message::ANSWER][i].RDLENGTH << std::endl;
        std::cout << "RDATA     : " << std::endl;
        for (int j = 0; j < RR[message::ANSWER][i].RDLENGTH; j++) {
            std::cout << (unsigned) RR[message::ANSWER][i].RDATA[j];
            if (j == RR[message::ANSWER][i].RDLENGTH - 1) {
                std::cout << std::endl;
            } else {
                std::cout << ", ";
            }
        }
    }

    std::cout << "-----------------Authority----------------" << std::endl;
    for (int i = 0; i < RR[message::AUTHORITY].size(); i++) {
        std::cout << "[authority " << i << "]" << std::endl;
        std::cout << "NAME      : ";
        debug_str(RR[message::AUTHORITY][i].NAME);
        std::cout << "TYPE      : " << RR[message::AUTHORITY][i].TYPE << std::endl;
        std::cout << "CLASS     : " << RR[message::AUTHORITY][i].CLASS << std::endl;
        std::cout << "TTL       : " << RR[message::AUTHORITY][i].TTL << std::endl;
        std::cout << "RDLENGTH  : " << RR[message::AUTHORITY][i].RDLENGTH << std::endl;
        std::cout << "RDATA     : " << std::endl;
        for (int j = 0; j < RR[message::AUTHORITY][i].RDLENGTH; j++) {
            std::cout << (unsigned)RR[message::AUTHORITY][i].RDATA[j];
            if (j == RR[message::AUTHORITY][i].RDLENGTH - 1) {
                std::cout << std::endl;
            } else {
                std::cout << ", ";
            }
        }
    }

    std::cout << "----------------Additional----------------" << std::endl;
    for (int i = 0; i < RR[message::ADDTIONAL].size(); i++) {
        std::cout << "[additional " << i << "]" << std::endl;
        std::cout << "NAME      : ";
        debug_str(RR[message::ADDTIONAL][i].NAME);
        std::cout << "TYPE      : " << RR[message::ADDTIONAL][i].TYPE << std::endl;
        std::cout << "CLASS     : " << RR[message::ADDTIONAL][i].CLASS << std::endl;
        std::cout << "TTL       : " << RR[message::ADDTIONAL][i].TTL << std::endl;
        std::cout << "RDLENGTH  : " << RR[message::ADDTIONAL][i].RDLENGTH << std::endl;
        std::cout << "RDATA     : " << std::endl;
        for (int j = 0; j < RR[message::ADDTIONAL][i].RDLENGTH; j++) {
            std::cout << (unsigned) RR[message::ADDTIONAL][i].RDATA[j] << std::endl;
            if (j == RR[message::ADDTIONAL][i].RDLENGTH - 1) {
                std::cout << std::endl;
            } else {
                std::cout << ", ";
            }
        }
    }
}

void message::debug_str(std::string str) {
    int cnt = 0;
    for (int i = 0; i < str.length(); i++) {
        if (!cnt) {
            cnt = str[i];
            std::cout << (char)(str[i] + '0');
        }
        else {
            cnt--;
            std::cout << str[i];
        }
    }
    std::cout << std::endl;
}