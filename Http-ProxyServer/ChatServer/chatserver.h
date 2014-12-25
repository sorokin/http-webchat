#ifndef CHATSERVER_H
#define CHATSERVER_H
#include <application.h>
#include <Http/httpserver.h>
#include <sstream>
#include <cstring>
#include <iostream>
#include <fstream>
#include <ctime>
using namespace std;

class ChatServer
{
public:
    ChatServer(Application* app);
    ~ChatServer();
    int start(int port);

    class Message {
    public:
        Message(int from, int time, const std::string& text):
            from(from), time(time), text(text) {}
        int from;
        int time;
        std::string text;
        std::string toJson();
    };

private:
    const char* COOKIE_USER;
    const char* COOKIE_HASH;

    typedef unsigned int UserType;
    HttpServer *httpServer;
    //void addStaticHandler(const RouteMatcher& matcher, const std::string& filename);
    std::string getStringByFile(const char* name);
    UserType numUsers;
    UserType hash(UserType userId);
    std::map<UserType, int> firstReadMessage, lastReadMessage;

    vector <Message> history;
    UserType getUserIdByCookie(std::string cookie);
    std::string getMessage(const std::string& str);
    std::string packageToJsonHistory(int l, int r);

};

#endif // CHATSERVER_H
