#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <QUrl>
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
        Message(string from, time_t time, const std::string& text):
            from(from), time(time), text(text) {}
        string from;
        time_t time;
        std::string text;
        std::string toJson();
    };

private:
    HttpServer* httpServer;
    vector <Message> history;
    std::map<string, size_t> firstMessage, firstUnreadMessage;

    static std::string getMessage(const std::string& str);
    std::string packageToJsonHistory(size_t l, size_t r);
    static void logError(const HttpRequest& req, int code, const string& res);
};

#endif // CHATSERVER_H
