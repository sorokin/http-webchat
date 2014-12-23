#include "httpserver.h"

#include <iostream>
using namespace std;

HttpServer::HttpServer(Application *app):app(app), listener(TcpServerSocket(app)) {}

void HttpServer::setMethodHandler(String method, const MethodHandler &handler) {
    method = HttpUtils::toLower(method);
    methodHandlers[method] = handler;
}

void HttpServer::setRouteHandler(std::string rout, const RouteHandler &handler) {
    HttpUtils::transformRoute(rout);
    routeHandlers[rout] = handler;
}

HttpServer::ServerStatus HttpServer::start(int port) {
    TcpServerSocket::ConnectedState e = listener.listen("127.0.0.1", port, [this]()
    {
        readRequest(listener.getPendingConnecntion());
    });

    if (e == TcpServerSocket::AlreadyBinded)
        return AlreadyBinded;
    if (e == TcpServerSocket::AlreadyConnected)
        return AlreadyStarted;
    return Success;
}


HttpServer::Response::Response(TcpSocket* socket):socket(socket) {}

bool HttpServer::Response::response(const HttpResponse& response) {
    if (socket != NULL) {
        socket->write(response.toString());
        socket = NULL;
        return true;
    }
    return false;
}

void HttpServer::readRequest(TcpSocket *socket) {
    HttpUtils::readHttp(socket, [=](HttpObject* tmp)
    {
        if (tmp != NULL) {
            HttpRequest *request = (HttpRequest*)tmp;
            String m = request->method();
            m = HttpUtils::toLower(m);
            if (methodHandlers.find(m) != methodHandlers.end());
                methodHandlers[m](*request, Response(socket));
            if (!request->isKeepAlive())
                delete socket;
        }
        sockets.insert(socket);
    },
    []() {return new HttpRequest(HttpObject::Dynamic);});
}

HttpServer::~HttpServer() {
    for (set <TcpSocket*>::iterator it = sockets.begin(); it != sockets.end(); it++)
        delete *it;
}

