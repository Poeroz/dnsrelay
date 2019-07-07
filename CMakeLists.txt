cmake_minimum_required(VERSION 3.12)
project(dnsrelay)

set(CMAKE_CXX_STANDARD 14)

add_executable(dnsrelay main.cpp nameTable.cpp nameTable.h socketManager.cpp socketManager.h parser.cpp parser.h message.cpp message.h socketHeader.h recordTable.cpp recordTable.h)