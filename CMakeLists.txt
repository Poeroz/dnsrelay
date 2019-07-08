cmake_minimum_required(VERSION 3.12)
project(dnsrelay)

set(CMAKE_CXX_STANDARD 14)

add_executable(dnsrelay src/main.cpp src/nameTable.cpp lib/nameTable.h src/socketManager.cpp lib/socketManager.h src/parser.cpp lib/parser.h src/message.cpp lib/message.h lib/socketHeader.h src/recordTable.cpp lib/recordTable.h)