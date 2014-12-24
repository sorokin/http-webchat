#include "chatserver.h"

ChatServer::ChatServer(Application* app):httpServer(new HttpServer(app))
{
    addStaticHandler(RouteMatcher("GET", "/"), "index.html");
    addStaticHandler(RouteMatcher("GET", "/script.js"), "script.js");
    addStaticHandler(RouteMatcher("GET", "/jquery.js"), "jquery.js");

    /*httpServer->setRouteHandler("/messages", [](HttpRequest req, HttpServer::Response resp) {
        HttpResponse r(200, "OK", req.version(), "");
        if (req.isKeepAlive())
            r.setHeader("Connection", "Keep-Alive");
        resp.response(r);
    });*/
}

void ChatServer::addStaticHandler(const RouteMatcher& matcher, const char* filename) {
    httpServer->addRouteMatcher(matcher, [=](HttpRequest req, HttpServer::Response resp) {
        HttpResponse r(200, "OK", req.version(), getStringByFile(filename));
        if (req.isKeepAlive())
            r.setHeader("Connection", "Keep-Alive");
        resp.response(r);
    });
}

int ChatServer::start(int port) {
    return httpServer->start(port);
}


std::string ChatServer::getStringByFile(const char* name) {
    ifstream in(name);
    std::string s, ret;
    while (getline(in, s))
        ret += s + "\n";
    return ret;
}


ChatServer::~ChatServer()
{
    delete httpServer;
}


