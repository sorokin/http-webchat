#ifndef HTTPUTILS_H
#define HTTPUTILS_H

#include <algorithm>
#include <functional>

#include <QUrl>

#include "../TCP-socket/tcpsocket.h"

#include "httpobject.h"
#include "httprequest.h"

namespace HttpUtils
{
    void readHttp(TcpSocket *socket,
                  const std::function <void(HttpObject*)>& onFinish,
                  const std::function <HttpObject*()>& creator);

    std::string toLower(std::string s);
    std::string transformRoute(std::string route);
}

#endif // HTTPUTILS_H
