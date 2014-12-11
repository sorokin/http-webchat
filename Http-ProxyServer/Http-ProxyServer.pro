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
    TCP-socket/tcpserversocket.cpp \
    Http/httprequest.cpp \
    Http/httpresponse.cpp \
    Http/httpclient.cpp \
    Http/httpserver.cpp \
    Http/httpobject.cpp \
    Http/httputils.cpp

HEADERS += \
    application.h \
    TCP-socket/tcpsocket.h \
    TCP-socket/tcpserversocket.h \
    Http/httprequest.h \
    Http/httpresponse.h \
    Http/httpclient.h \
    Http/httpserver.h \
    Http/httpobject.h \
    Http/httputils.h

QMAKE_CXXFLAGS += -std=c++11
