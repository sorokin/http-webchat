cmake_minimum_required(VERSION 3.3)
project(HttpWebChat)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        main.cpp
        TCPSocket/tcp_accept_socket.cpp
        TCPSocket/tcp_accept_socket.h
        TCPSocket/tcp_socket.cpp
        TCPSocket/tcp_socket.h
        poller.cpp
        poller.h
        common.h TCPSocket/tcp_server_socket.cpp TCPSocket/tcp_server_socket.h)

add_executable(HttpWebChat ${SOURCE_FILES} main.cpp)