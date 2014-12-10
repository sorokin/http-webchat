#include "httpclient.h"
#include <QDebug>
#include <sstream>

#include <iostream>
using namespace std;

HttpClient::HttpClient(Application *app):app(app) {}

HttpClient::StatusRequest HttpClient::request(const HttpRequest& request, const ResponseHandler& handler) {
    const HttpRequest::String END_LINE = "\r\n";

    TcpSocket *socket = new TcpSocket(app);
    TcpSocket::ConnectedState er = socket->connectToHost(request.host());
    if (er == TcpSocket::UnknownHost) {
        delete socket;
        return UnknownHost;
    }

    socket->setClosedConnectionHandler([=]() {
        if (handler)
            handler(parseResponse(socket->readBytes()));
        delete socket;
    });

    HttpRequest::String msg = request.method() + " " + request.path() +
            " HTTP/" + request.version() + END_LINE;
    HttpRequest::HeadersContainer headers = request.headers();
    for (HttpRequest::HeadersContainer::iterator it = headers.begin(); it != headers.end(); it++)
        msg += it->first + ": " + it->second + END_LINE;
    msg += END_LINE;
    msg += request.messageBody();
    socket->write(msg.c_str(), msg.size());
    return Success;
}

HttpResponse HttpClient::parseResponse(const TcpSocket::Bytes& s) {
    std::stringstream in(s);
    std::string protVersion, reasPhrase;
    int code;
    in >> protVersion >> code >> reasPhrase;
    protVersion.erase(0, 5);
    HttpResponse ret(code, reasPhrase, protVersion);
    std::string line;
    getline(in, line);
    while (getline(in, line)) {
        int pos = line.find(":");
        string key = line.substr(0, pos);
        string value = (pos + 2 < s.size() ? s.substr(pos + 2, s.size() - pos - 2) : "");
        ret.setHeader(key, value);
    }
    HttpResponse::Data sum, l;
    while (getline(in, l))
        sum += l;
    ret.setMessageBody(s);
    return ret;
}
