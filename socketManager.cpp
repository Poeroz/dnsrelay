//
// Created by Poeroz on 2019/7/5.
//

#include <cstring>
#include <chrono>
#include "socketManager.h"

socketManager::socketManager() {
    hostSocket = socket(PF_INET, SOCK_DGRAM, UDP_PROTOCOL);
    if (hostSocket < 0) {
        ERR_EXIT("socket error");
    }
    else {
        sockaddr_in hostAddr;
        hostAddr.sin_family = AF_INET;
        hostAddr.sin_port = htons(UDP_PORT);
        hostAddr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(hostSocket, (sockaddr *)&hostAddr, sizeof(sockaddr_in)) < 0) {
            ERR_EXIT("bind error");
        }

        std::cout << "socket initialized successfully." << std::endl;
    }
}

int socketManager::recvBuffer(uint8_t *buffer, sockaddr_in &senderAddr, time_t &recvTime) {
    memset(buffer, 0, MAX_LEN);
    socklen_t addrLen = sizeof(sockaddr_in);
    recvTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    int recvLen = recvfrom(hostSocket, buffer, MAX_LEN, 0, (sockaddr *)&senderAddr, &addrLen);
    if (recvLen <= 0) {
        ERR_EXIT("connect error");
    }
    else {
        return recvLen;
    }
}

void socketManager::sendBuffer(uint8_t *buffer, int bufferSize, sockaddr_in recvAddr) {
//    std::cout << "send address" << std::endl;
//    std::cout << recvAddr.sin_addr.s_addr << std::endl;
//    std::cout << recvAddr.sin_port << std::endl;

    int status = sendto(hostSocket, buffer, bufferSize, 0, (const sockaddr *)&recvAddr, sizeof(sockaddr_in));
    std::cout << "send status : ";
    std::cout << status << std::endl;
    if (status < 0) {
        ERR_EXIT("send error");
    }
}