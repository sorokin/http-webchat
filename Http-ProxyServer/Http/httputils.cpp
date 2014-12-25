#include "httputils.h"

#include <cstdio>
#include <iostream>
using namespace std;


void HttpUtils::readHttp(TcpSocket *socket,
              const std::function <void(HttpObject*)>& onFinish,
              const std::function <HttpObject*()>& creator) {
    HttpObject *obj = NULL;
    socket->setDataReceivedHandler([=]() mutable
    {
        if (obj == NULL)
            obj = creator();

        obj->append(socket->readBytes());
        if (obj->hasBody() && (int)obj->body().size() == obj->contentLength()) {
            obj->commit();
            onFinish(obj);
            delete obj;
            obj = NULL;
        }
    });

    socket->setClosedConnectionHandler([=]() mutable
    {
        if (obj != NULL)
            obj->commit();
        onFinish(obj);
        if (obj != NULL) {
            delete obj;
            obj = NULL;
        }
    });
}


std::string HttpUtils::toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

std::string HttpUtils::transformRoute(string route) {
    if (route == "")
        return route;
    std::transform(route.begin(), route.end(), route.begin(), ::tolower);
    QUrl url(route.c_str());
    QString qstr = url.path();
    if (qstr[qstr.size() - 1] == '/' && qstr.size() != 1)
        qstr.remove(qstr.size() - 1, 1);
    route = qstr.toStdString();
    return route;
}
