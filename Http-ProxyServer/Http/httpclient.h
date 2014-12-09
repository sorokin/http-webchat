#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H
#include "Http/httprequest.h"
#include <Http/httpresponse.h>
#include <TCP-socket/tcpsocket.h>
#include <functional>

class HttpClient
{
    HttpResponse parseResponse(const TcpSocket::Bytes& s);
public:
    HttpClient();
    typedef std::function<void (HttpResponse)> ResponseHandler;
    void request(const HttpRequest& request, const ResponseHandler &handler = ResponseHandler());
};

#endif // HTTPCLIENT_H
