#include "httpserver.h"

#include <iostream>
using namespace std;

HttpServer::HttpServer(Application *app):app(app), listener(TcpServerSocket(app)) {}

void HttpServer::addRouteMatcher(const RouteMatcher& matcher, const RequestHandler &handler) {
    matchers.push_back(make_pair(matcher, handler));
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
            cerr << " request = " << request->toString() << endl;
            for (int i = 0; i < (int)matchers.size(); ++i)
                if (matchers[i].first.match(*request))
                    matchers[i].second(*request, Response(socket));
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

