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
    ChatServer/chatserver.cpp \
    Http/httpclient.cpp \
    Http/httpobject.cpp \
    Http/httprequest.cpp \
    Http/httpresponse.cpp \
    Http/httpserver.cpp \
    Http/httputils.cpp \
    Http/routematcher.cpp \
    TCP-socket/tcpsocket.cpp \
    TCP-socket/tcpserversocket.cpp

HEADERS += \
    application.h \
    ChatServer/chatserver.h \
    Http/httpclient.h \
    Http/httpobject.h \
    Http/httprequest.h \
    Http/httpresponse.h \
    Http/httpserver.h \
    Http/httputils.h \
    Http/routematcher.h \
    TCP-socket/tcpsocket.h \
    TCP-socket/tcpserversocket.h \

QMAKE_CXXFLAGS += -std=c++11
