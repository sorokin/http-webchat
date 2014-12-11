#ifndef HTTPUTILS_H
#define HTTPUTILS_H
#include <TCP-socket/tcpsocket.h>
#include <Http/httpobject.h>
#include <functional>

#include <iostream>
using namespace std;

class HttpUtils
{
    HttpUtils();
public:
    static void readHttp(TcpSocket *socket, HttpObject* object,
                         const std::function <void()>& onFinish) {
        socket->setDataReceivedHandler([=]()
        {
            object->append(socket->readBytes());
            if (object->isBody() && object->body().size() == object->contentLength()) {
                object->commit();
                onFinish();
            }
        });

        socket->setClosedConnectionHandler([=]()
        {
            object->commit();
            onFinish();
        });
    }
};

#endif // HTTPUTILS_H
