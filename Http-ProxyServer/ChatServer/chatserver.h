#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <QUrlQuery>

#include "../application.h"
#include "../Http/httprequest.h"
#include "../Http/httpresponse.h"
#include "../Http/httpserver.h"
#include "../Http/routematcher.h"

using namespace std;

class ChatServer
{
public:
    ChatServer(Application* app);
    ~ChatServer();
    int start(uint16_t port);

    class Message {
    public:
        Message(int from, time_t time, const std::string& text):
            from(from), time(time), text(text) {}
        int from;
        time_t time;
        std::string text;
        std::string toJson();
    };

private:
    const char* COOKIE_USER;
    const char* COOKIE_HASH;

    typedef unsigned int UserType;
    HttpServer* httpServer;
    //void addStaticHandler(const RouteMatcher& matcher, const std::string& filename);
//    std::string getStringByFile(const char* name);
    UserType numUsers;
//    UserType hash(UserType userId);
    std::map<UserType, size_t> firstReadMessage, lastReadMessage;

    vector <Message> history;
    UserType getUserIdByCookie(std::string cookie);
    std::string getMessage(const std::string& str);
    std::string packageToJsonHistory(size_t l, size_t r);

    hash<UserType> s_hash;
};

#endif // CHATSERVER_H
