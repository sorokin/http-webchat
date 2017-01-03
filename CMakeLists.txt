cmake_minimum_required(VERSION 3.3)
project(HttpWebChat)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        main.cpp
        HTTP/http_server.cpp
        HTTP/http_server.h
        HTTP/route_matcher.cpp
        HTTP/route_matcher.h
        HTTP/http_request.cpp
        HTTP/http_request.h
        HTTP/http_response.cpp
        HTTP/http_response.h
        HTTP/http_message.cpp
        HTTP/http_message.h
        HTTP/http_common.cpp
        HTTP/http_common.h
        TCPSocket/tcp_accept_socket.cpp
        TCPSocket/tcp_accept_socket.h
        TCPSocket/tcp_server_socket.cpp
        TCPSocket/tcp_server_socket.h
        TCPSocket/tcp_socket.cpp
        TCPSocket/tcp_socket.h
        poller.cpp
        poller.h
        common.cpp
        common.h ChatServer/chat_server.cpp ChatServer/chat_server.h ChatServer/json.cpp ChatServer/json.h)

add_executable(HttpWebChat ${SOURCE_FILES} main.cpp)