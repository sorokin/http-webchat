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

void HttpServer::stop() {
}


void HttpServer::readRequest(TcpSocket *socket) {
    cerr << socket << endl;
    socket->setDataReceivedHandler([=]()
    {
        TcpSocket::Bytes s = socket->readBytes();
        cerr << s << "#";
        cerr << (int)s[s.size() - 2] << " " << (int)s[s.size() - 1];
    });

    socket->setClosedConnectionHandler([=]()
    {
        cout << socket->readBytes() << endl;
    });
}
