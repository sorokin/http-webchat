#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include <application.h>
#include <TCP-socket/tcpserversocket.h>
#include <functional>
#include <map>
#include <string>
#include <cstring>
#include <algorithm>
#include <Http/httputils.h>
#include <Http/httprequest.h>
#include <Http/httpresponse.h>
#include <memory>

class HttpServer
{
    typedef std::string String;
public:
    class Response {
        Response(TcpSocket* socket);
        TcpSocket *socket;
    public:
        bool response(const HttpResponse& response);
        friend class HttpServer;
    };

    enum ServerStatus {Success, AlreadyBinded, AlreadyStarted};
    typedef std::function <void()> RouteHandler;
    typedef std::function <void(HttpRequest, Response)> MethodHandler;

    HttpServer(Application* app);
    ServerStatus start(int port);
    void setRouteHandler(String rout, const RouteHandler& handler);
    void setMethodHandler(String method, const MethodHandler& handler);
protected:
    Application *app;
    std::map <String, MethodHandler> methodHandlers;
    std::map <String, RouteHandler> routeHandlers;
    TcpServerSocket *listener;
    void readRequest(TcpSocket *socket);

    void transformRoute(String& route);
    void transformMethod(String& method);
};

#endif // HTTPSERVER_H
