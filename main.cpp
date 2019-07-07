#include "parser.h"

int main() {
    message msg;
    msg.header.ID = 2333;
    msg.header.QR = 1;
    msg.header.OPCODE = 4;
    msg.header.AA = 0;
    msg.header.TC = 1;
    msg.header.RD = 0;
    msg.header.RA = 1;
    msg.header.RCODE = 12;
    msg.header.QDCOUNT = 1;
    msg.header.RRCOUNT[message::ANSWER] = 2;
    msg.header.RRCOUNT[message::AUTHORITY] = 3;
    msg.header.RRCOUNT[message::ADDTIONAL] = 4;
    for (int i = 0; i < msg.header.QDCOUNT; i++) {
        message::QUESTION tmp;
        tmp.QNAME = "question";
        tmp.QTYPE = tmp.QCLASS = 1;
        msg.question.push_back(tmp);
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < msg.header.RRCOUNT[i]; j++) {
            message::RESOURCE_RECORD tmp;
            tmp.NAME = "RR";
            tmp.TYPE = tmp.CLASS = 1;
            tmp.TTL = 20;
            tmp.RDLENGTH = 1;
            tmp.RDATA.clear();
            tmp.RDATA.push_back((uint8_t)23);
            msg.RR[i].push_back(tmp);
        }
    }

    uint8_t buf[MAX_LEN];
    int buflen = 0;
    msg.struct2Buffer(buf, buflen);
    std::cout << *(uint16_t *)buf << std::endl;
    sockaddr_in senderAddr;
    message msg_tmp = message(buf, buflen, senderAddr);
    msg_tmp.debug();


    parser *p = new parser();

    while (true) {
        p->receive();
    }
    return 0;
}