#include "httpserver.h"

using namespace std;

HttpServer::HttpServer(Application *app):app(app), listener(TcpServerSocket(app)) {}

void HttpServer::addRouteMatcher(const RouteMatcher& matcher, const RequestHandler &handler) {
    matchers.push_back(make_pair(matcher, handler));
}

void HttpServer::start(uint16_t port) {
//    TcpServerSocket::ConnectedState e = listener.listen("127.0.0.1", port, [this]()
//    {
//        readRequest(listener.getPendingConnection());
//    });

    listener.listen("127.0.0.1", port, [this](TcpSocket *socket) {
        readRequest(socket);
    });

//    if (e == TcpServerSocket::AlreadyConnected)
//        return AlreadyStarted;
//    if (e != TcpServerSocket::Success)
//        return StartError;
//    return Success;
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
    if (socket == NULL) {
        return;
    }
    HttpUtils::readHttp(socket, [=](HttpObject* tmp)
    {
        if (tmp != NULL) {
            HttpRequest *request = (HttpRequest*)tmp;
            bool found = false;
            for (int i = 0; i < (int)matchers.size(); ++i)
                if (matchers[i].first.match(*request)) {
                    matchers[i].second(*request, Response(socket));
                    found = true;
                }

            if (!found)
                for (int i = 0; i < (int)matchers.size(); ++i)
                    if (matchers[i].first.method() == "" && matchers[i].first.url() == "")
                        matchers[i].second(*request, Response(socket));

            if (!request->notKeptAlive())
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

