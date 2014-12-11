#include "httpserver.h"

#include <iostream>
using namespace std;

HttpServer::HttpServer(Application *app):app(app), listener(new TcpServerSocket(app)) {}

void HttpServer::transformMethod(String& method) {
    std::transform(method.begin(), method.end(), method.begin(), ::tolower);
}

void HttpServer::transformRoute(String& route) {
    std::transform(route.begin(), route.end(), route.begin(), ::tolower);
    QUrl url(route.c_str());
    QString qstr = url.path();
    if (qstr[qstr.size() - 1] == '/' && qstr.size() != 1)
        qstr.remove(qstr.size() - 1, 1);
    route = qstr.toStdString();
}

void HttpServer::setMethodHandler(String method, const MethodHandler &handler) {
    transformMethod(method);
    methodHandlers[method] = handler;
}

void HttpServer::setRouteHandler(std::string rout, const RouteHandler &handler) {
    transformRoute(rout);
    routeHandlers[rout] = handler;
}

HttpServer::ServerStatus HttpServer::start(int port) {
    initializate();
    TcpServerSocket::ConnectedState e =
            listener->listen("127.0.0.1", port, [this]() {readRequest(listener->getPendingConnecntion());});
    if (e == TcpServerSocket::AlreadyBinded)
        return AlreadyBinded;
    if (e == TcpServerSocket::AlreadyConnected)
        return AlreadyStarted;
    return Success;
}


HttpServer::Response::Response(TcpSocket* socket): socket(socket),
    alreadyResponsed(alreadyResponsed) {}

bool HttpServer::Response::response(const HttpResponse& response) {
    if (!alreadyResponsed) {
        socket->write(response.toString().c_str(), response.toString().size());
        alreadyResponsed = true;
        return true;
    }
    return false;
}

void HttpServer::readRequest(TcpSocket *socket) {
    HttpRequest *request = new HttpRequest(HttpObject::Dynamic);
    HttpUtils::readHttp(socket, request, [=]()
    {
        String m = request->method();
        transformMethod(m);
        if (methodHandlers.find(m) != methodHandlers.end()) {
            cerr << "in handler\n";
            methodHandlers[request->method()](*request, Response(socket));
        }
    });
}
