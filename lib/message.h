//
// Created by Poeroz on 2019/7/5.
//

#ifndef DNSRELAY_MESSAGE_H
#define DNSRELAY_MESSAGE_H


#include <string>
#include <cstdint>
#include <vector>
#include <iostream>
#include "socketHeader.h"

class message {
public:

//                                    1  1  1  1  1  1
//      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                      ID                       |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |QR|   Opcode  |AA|TC|RD|RA|        |   RCODE   |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                    QDCOUNT                    |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                    ANCOUNT                    |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                    NSCOUNT                    |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                    ARCOUNT                    |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

    struct HEADER {
        uint16_t ID;

        bool QR;
        uint8_t OPCODE;
        bool AA;
        bool TC;
        bool RD;
        bool RA;
        uint8_t ZERO;
        uint8_t RCODE;

        uint16_t QDCOUNT;
        uint16_t RRCOUNT[3];
    };

//                                    1  1  1  1  1  1
//      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                                               |
//    /                     QNAME                     /
//    /                                               /
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                     QTYPE                     |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                     QCLASS                    |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

    struct QUESTION {
        std::string QNAME;
        uint16_t QTYPE;
        uint16_t QCLASS;
    };

//                                    1  1  1  1  1  1
//      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                                               |
//    /                                               /
//    /                      NAME                     /
//    |                                               |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                      TYPE                     |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                     CLASS                     |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                      TTL                      |
//    |                                               |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                   RDLENGTH                    |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--|
//    /                     RDATA                     /
//    /                                               /
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

    struct RESOURCE_RECORD {
        std::string NAME;
        uint16_t TYPE;
        uint16_t CLASS;
        int32_t TTL;
        uint16_t RDLENGTH;
        std::vector<uint8_t> RDATA;
    };

    enum RRTYPE {
        ANSWER, AUTHORITY, ADDTIONAL
    };

    std::vector<QUESTION> question;
    std::vector<RESOURCE_RECORD> RR[3];
    HEADER header;
    uint8_t *startPos;
    sockaddr_in senderAddr;

    message() = default;
    message(uint8_t *buffer, int bufferSize, sockaddr_in senderAddr);

    static std::string transName(std::string name);

    void buffer2Struct(uint8_t *ptr);
    void struct2Buffer(uint8_t *ptr, int &bufferSize);

    void debug();


private:
    void debug_str(std::string str);
    void unpackName(uint8_t *&ptr, std::string &name);

    void buffer2Header(uint8_t *&ptr);
    void buffer2Question(uint8_t *&ptr);
    void buffer2RR(uint8_t *&ptr, message::RRTYPE type);

    void header2Buffer(uint8_t *&ptr, int &bufferSize);
    void question2Buffer(uint8_t *&ptr, int &bufferSize);
    void RR2Buffer(uint8_t *&ptr, int &bufferSize, RRTYPE type);

    void getUint16(uint16_t &var, uint8_t *&ptr);
    void getInt32(int32_t &var, uint8_t *&ptr);

    void putUint16(uint16_t var, uint8_t *&ptr, int &bufferSize);
    void putInt32(int32_t var, uint8_t *&ptr, int &bufferSize);};

#endif //DNSRELAY_MESSAGE_H