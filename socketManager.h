//
// Created by Poeroz on 2019/7/5.
//

#ifndef DNSRELAY_SOCKETMANAGER_H
#define DNSRELAY_SOCKETMANAGER_H

#include "socketHeader.h"
#include "message.h"
#include <iostream>

#define ERR_EXIT(m) \
    do { \
    perror(m); \
    exit(EXIT_FAILURE); \
    } while(0)

#define SOCKET int
#define UDP_PORT 53
#define UDP_PROTOCOL 0
#define SERVER_DNS_IP "10.3.9.4"
#define MAX_LEN 1024

class socketManager {
public:
    socketManager();
    ~socketManager();
    void sendBuffer(uint8_t *buffer, int bufferSize, sockaddr_in recvAddr);
    int recvBuffer(uint8_t *buffer, sockaddr_in &senderAddr, time_t &recvTime);
    SOCKET hostSocket, serverSocket;
};


#endif //DNSRELAY_SOCKETMANAGER_H
