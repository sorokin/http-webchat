cmake_minimum_required(VERSION 3.3)
project(HttpWebChat)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        main.cpp
        HTTP/http_message.cpp
        HTTP/http_message.h
        TCPSocket/tcp_accept_socket.cpp
        TCPSocket/tcp_accept_socket.h
        TCPSocket/tcp_server_socket.cpp
        TCPSocket/tcp_server_socket.h
        TCPSocket/tcp_socket.cpp
        TCPSocket/tcp_socket.h
        poller.cpp
        poller.h
        common.h HTTP/http_request.cpp HTTP/http_request.h common.cpp HTTP/http_response.cpp HTTP/http_response.h)

add_executable(HttpWebChat ${SOURCE_FILES} main.cpp)