#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H
#include <Http/httpobject.h>

class HttpRequest: public HttpObject
{
public:
    enum Method {HEAD, GET, PUT, POST, DELETE};
    HttpRequest();
    HttpRequest(Method method, const String& url,
                const Data& body = "", const String& version = "1.0");
    HttpRequest(const String& method, const String& url,
                const Data& body = "", const String& version = "1.0");
    String url() const;
    void setUrl(const String& url);
    void setMethod(const String& method);
    void setMethod(Method method);
    String method() const;

    String host() const;
    String path() const;
    ~HttpRequest();
private:
    String getMethod(Method method);
    void splitUrl(String url, String& host, String& path);
    String mMethod;
    String mHost;
    String mPath;
};

#endif // HTTPREQUEST_H
