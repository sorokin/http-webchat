cmake_minimum_required(VERSION 3.3)
project(HttpWebChat)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        main.cpp
        poller.cpp
        poller.h
        common.h)

add_executable(HttpWebChat ${SOURCE_FILES} main.cpp)