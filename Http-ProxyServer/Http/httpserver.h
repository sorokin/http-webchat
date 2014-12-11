#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include <application.h>
#include <TCP-socket/tcpserversocket.h>
#include <functional>
#include <map>
#include <string>
#include <cstring>
#include <algorithm>
#include <QUrl>
#include <Http/httputils.h>
#include <Http/httprequest.h>

class HttpServer
{
private:
    typedef std::string String;
public:
    enum ServerStatus {Success, AlreadyBinded, AlreadyStarted};
    typedef std::function <void()> RouteHandler;
    typedef std::function <void()> MethodHandler;

    HttpServer(Application* app);
    ServerStatus start(int port);
    void stop();
    void setRouteHandler(String rout, const RouteHandler& handler);
    void setMethodHandler(String method, const MethodHandler& handler);
protected:
    virtual void initializate() = 0;
private:
    Application *app;
    std::map <String, MethodHandler> methodHandlers;
    std::map <String, RouteHandler> routeHandlers;
    TcpServerSocket *listener;
    void readRequest(TcpSocket *socket);

    void transformRoute(String& route);
    void transformMethod(String& method);
};

#endif // HTTPSERVER_H
