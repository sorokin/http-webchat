#include "httpclient.h"
#include <TCP-socket/tcpsocket.h>
#include <QDebug>

#include <iostream>
using namespace std;
HttpClient::HttpClient() {}

void HttpClient::request(const HttpRequest& request, const ResponseHandler& handler) {
    TcpSocket *socket = new TcpSocket();
    printf("in\n");
    socket->connectToHost("google.ru", 80);
    //socket.write(request.data());
    socket->setDataReceivedHandler([=]() {
            //qDebug() << QString(socket->readBytes());
            std::string data = socket->readString();
            qDebug() << QString(data.c_str());
            delete socket;
    });
    socket->write("GET / HTTP/1.0\nHost: www.google.ru\n\n");
}
