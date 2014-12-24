#ifndef CHATSERVER_H
#define CHATSERVER_H
#include <application.h>
#include <Http/httpserver.h>
#include <sstream>
#include <cstring>
#include <iostream>
#include <fstream>
using namespace std;

class ChatServer
{
public:
    ChatServer(Application* app);
    ~ChatServer();
    int start(int port);
private:
    HttpServer *httpServer;
    void addStaticHandler(const RouteMatcher& matcher, const std::string& filename);
    std::string getStringByFile(const char* name);
};

#endif // CHATSERVER_H
