#include "httpclient.h"
#include <TCP-socket/tcpsocket.h>
#include <QDebug>

HttpClient::HttpClient() {}

void HttpClient::request(const HttpRequest& request, const ResponseHandler& handler) {
    const HttpRequest::String END_LINE = "\r\n";

    TcpSocket *socket = new TcpSocket();
    socket->connectToHost("google.ru");
    socket->setDataReceivedHandler([=]() {
            std::string data = socket->readString();
            qDebug() << QString(data.c_str());
    });

    HttpRequest::String msg = request.method() + " " + request.path() +
            " HTTP/" + request.version() + END_LINE;
    HttpRequest::HeadersContainer headers = request.headers();
    for (HttpRequest::HeadersContainer::iterator it = headers.begin(); it != headers.end(); it++)
        msg += it->first + ": " + it->second + END_LINE;
    msg += END_LINE;
    if (request.messageBody() != NULL)
        msg += request.messageBody();
    //printf("%s", msg.c_str());
    socket->write(msg.c_str(), msg.size());
}
