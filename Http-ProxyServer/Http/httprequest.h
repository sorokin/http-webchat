#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H
#include <cstring>
#include <string>
#include <map>
#include <vector>

class HttpRequest
{
public:
    typedef std::string String;
    typedef std::map <String, String> HeadersContainer;

    enum Method {HEAD, GET, PUT, POST, DELETE};
    HttpRequest();
    HttpRequest(Method method, const String& url, const char* body = NULL, const String& version = "1.0");
    HttpRequest(const String& method, const String& url, const char* body = NULL, const String& version = "1.0");
    void setMethod(const String& method);
    void setMethod(Method method);
    String method() const;
    void setUrl(const String& url);
    String url() const;
    void setVersion(const String& vers);
    String version() const;
    void setHeader(const String& key, const String& val);
    void setHeaders(const std::vector <std::pair <String, String> >& headers);
    HeadersContainer headers() const;
    String header(const String& key);
    void setMessageBody(const String& message);
    void setMessageBody(const char* message);
    const char* messageBody() const;
    String host() const;
    String path() const;
private:
    String getMethod(Method method);
    void splitUrl(String url, String& host, String& path);
    String mMethod;
    String mHost;
    String mPath;
    String mVersion;
    const char* mBody;
    HeadersContainer mHeaders;
};

#endif // HTTPREQUEST_H
