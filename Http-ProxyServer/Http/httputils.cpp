#include "httputils.h"

#include <cstdio>
#include <iostream>
using namespace std;


void HttpUtils::readHttp(TcpSocket *socket,
              const std::function <void(HttpObject*)>& onFinish,
              const std::function <HttpObject*()>& creator) {
    bool *alreadyFinished = new bool(false);
    HttpObject *object = creator();
    socket->setDataReceivedHandler([=]()
    {
        object->append(socket->readBytes());
        if (object->isBody() && object->body().size() == object->contentLength()) {
            object->commit();
            *alreadyFinished = true;
            onFinish(object);
        }
    });

    socket->setClosedConnectionHandler([=]()
    {
        if (!*alreadyFinished) {
            object->commit();
            onFinish(object);
        }
        delete alreadyFinished;
    });
}


std::string HttpUtils::toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

void HttpUtils::transformRoute(std::string& route) {
    std::transform(route.begin(), route.end(), route.begin(), ::tolower);
    QUrl url(route.c_str());
    QString qstr = url.path();
    if (qstr[qstr.size() - 1] == '/' && qstr.size() != 1)
        qstr.remove(qstr.size() - 1, 1);
    route = qstr.toStdString();
}
