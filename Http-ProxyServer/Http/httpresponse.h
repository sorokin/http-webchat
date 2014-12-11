#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <Http/httpobject.h>

class HttpResponse:public HttpObject
{
public:
    HttpResponse();
    HttpResponse(int statusCode, const String& reasonPhrase,
                 const String& version, const Data& message = "");

    void setUrl(const String& url);
    String url() const;
    void setStatusCode(int statusCode);
    int statusCode() const;
    void setReasonPhrase(const String& rph);
    String reasonPhrase() const;
    ~HttpResponse();
private:
    int mStatusCode;
    String mReasonPhrase;
    String mUrl;
};

#endif // HTTPRESPONSE_H
