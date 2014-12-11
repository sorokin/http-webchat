#include "httpclient.h"
#include <QDebug>
#include <sstream>

#include <iostream>
using namespace std;

HttpClient::HttpClient(Application *app):app(app) {}

HttpClient::RequestStatus HttpClient::request(const HttpRequest& request, const ResponseHandler& handler) {
    const HttpRequest::String END_LINE = "\r\n";

    TcpSocket *socket = new TcpSocket(app);
    TcpSocket::ConnectedState er = socket->connectToHost(request.host());
    if (er == TcpSocket::UnknownHost) {
        delete socket;
        return UnknownHost;
    }

    /*socket->setClosedConnectionHandler([=]() {
            cerr << "in close";
        if (handler) {
            HttpResponse ret(HttpResponse::Dynamic);
            ret.append(socket->readBytes());
            ret.commit();
            handler(ret);
        }
        delete socket;
    });*/

    HttpRequest::String msg = request.method() + " " + request.path() +
            " HTTP/" + request.version() + END_LINE;
    HttpRequest::HeadersContainer headers = request.headers();
    for (HttpRequest::HeadersContainer::iterator it = headers.begin(); it != headers.end(); it++)
        msg += it->first + ": " + it->second + END_LINE;
    msg += END_LINE;
    msg += request.body();
    socket->write(msg.c_str(), msg.size());
    return Success;
}
