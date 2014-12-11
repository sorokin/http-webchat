#include "httpclient.h"
#include <QDebug>
#include <sstream>

#include <iostream>
using namespace std;

HttpClient::HttpClient(Application *app):app(app) {}

HttpClient::RequestStatus HttpClient::request(const HttpRequest& request, const ResponseHandler& handler) {
    TcpSocket *socket = new TcpSocket(app);

    HttpResponse *obj = new HttpResponse(HttpObject::Dynamic);
    HttpUtils::readHttp(socket, obj, [=]()
    {
        if (handler)
            handler(*obj);
        delete obj;
        delete socket;
    });

    TcpSocket::ConnectedState er = socket->connectToHost(request.host());
    if (er == TcpSocket::UnknownHost) {
        delete socket;
        return UnknownHost;
    }

    socket->write(request.toString().c_str(), request.toString().size());
    return Success;
}
