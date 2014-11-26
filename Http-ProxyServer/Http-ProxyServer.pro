#-------------------------------------------------
#
# Project created by QtCreator 2014-11-14T23:27:49
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Http-ProxyServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    application.cpp \
    TCP-socket/tcpsocket.cpp \
    TCP-socket/tcpserversocket.cpp

HEADERS += \
    application.h \
    TCP-socket/tcpsocket.h \
    TCP-socket/tcpserversocket.h

QMAKE_CXXFLAGS += -std=c++11
