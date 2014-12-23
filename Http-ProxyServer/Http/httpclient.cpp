#include "httpclient.h"
#include <memory>

HttpClient::HttpClient(Application *app):app(app) {}

HttpClient::RequestStatus HttpClient::request(const HttpRequest& request,
                                              const ResponseHandler& handler) {
    TcpSocket *socket = new TcpSocket(app);
    HttpUtils::readHttp(socket, [=](HttpObject* tmp)
    {
        std::shared_ptr<HttpResponse> obj((HttpResponse*)tmp);
        if (handler)
            handler(*obj);
        delete socket;//TODO keep alive ?
    },
    [](){return new HttpResponse(HttpObject::Dynamic);});

    TcpSocket::ConnectedState er = socket->connectToHost(request.host());
    if (er == TcpSocket::UnknownHost) {
        delete socket;
        return UnknownHost;
    }

    socket->write(request.toString());
    return Success;
}
