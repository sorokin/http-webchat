#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H
#include "httprequest.h"
#include <Http/httpresponse.h>
#include <functional>

class HttpClient
{
public:
    HttpClient();
    typedef std::function<void (const HttpResponse&)> ResponseHandler;
    void request(const HttpRequest& request, const ResponseHandler &handler = ResponseHandler());
};

#endif // HTTPCLIENT_H
