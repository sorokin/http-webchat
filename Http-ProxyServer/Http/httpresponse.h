#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <Http/httpobject.h>

class HttpResponse:public HttpObject
{
public:
    HttpResponse(CreationMode mode = Static);
    HttpResponse(int statusCode, const String& reasonPhrase,
                 const String& version, const Data& message = "");

    void setUrl(const String& url);
    String url() const;
    void setStatusCode(int statusCode);
    int statusCode() const;
    void setReasonPhrase(const String& rph);
    String reasonPhrase() const;
    Data toString() const;

    ~HttpResponse();
protected:
    void parseFirstLine(const String &line);
private:

    int mStatusCode;
    String mReasonPhrase;
    String mUrl;
};

#endif // HTTPRESPONSE_H
