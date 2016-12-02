#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <algorithm>
#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>

#include "../application.h"
#include "../TCP-socket/tcpserversocket.h"
#include "../TCP-socket/tcpsocket.h"

#include "httprequest.h"
#include "httpresponse.h"
#include "httputils.h"
#include "routematcher.h"

class HttpServer
{
//    typedef std::string String;
public:

    class Response {
        Response(TcpSocket* socket);
        TcpSocket *socket;
    public:
        bool response(const HttpResponse& response);
        friend class HttpServer;
    };

    enum ServerStatus {Success, AlreadyBinded, AlreadyStarted};
    typedef std::function <void(HttpRequest, Response)> RequestHandler;

    HttpServer(Application* app);
    ServerStatus start(int port);
    void addRouteMatcher(const RouteMatcher& matcher, const RequestHandler &handler);
    ~HttpServer();
protected:
    std::set <TcpSocket*> sockets;
    Application *app;
    std::vector <pair <RouteMatcher, RequestHandler> > matchers;
    TcpServerSocket listener;
    void readRequest(TcpSocket *socket);
};

#endif // HTTPSERVER_H
