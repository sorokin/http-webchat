#ifndef HTTPUTILS_H
#define HTTPUTILS_H
#include <TCP-socket/tcpsocket.h>
#include <Http/httpobject.h>
#include <functional>
#include <algorithm>
#include <QUrl>
#include <Http/httprequest.h>

namespace HttpUtils
{
    void readHttp(TcpSocket *socket,
                  const std::function <void(HttpObject*)>& onFinish,
                  const std::function <HttpObject*()>& creator);

    std::string toLower(std::string s);
    std::string transformRoute(std::string route);
}

#endif // HTTPUTILS_H
