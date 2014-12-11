#ifndef HttpObject_H
#define HttpObject_H
#include <cstring>
#include <string>
#include <map>
#include <vector>

class HttpObject
{
public:
    typedef std::string Data;
    typedef std::string String;
    typedef std::map <String, String> HeadersContainer;

    HttpObject();
    HttpObject(const Data& body, const String& version);
    virtual void setUrl(const String& url) = 0;
    virtual String url() const = 0;
    void setVersion(const String& vers);
    String version() const;
    void setHeader(const String& key, const String& val);
    void setHeaders(const std::vector <std::pair <String, String> >& headers);
    String header(const String& key);
    HeadersContainer headers() const;
    void setBody(const String& message);
    void setBody(const char* message);
    Data body() const;
    virtual ~HttpObject();
protected:
    String mVersion;
    Data mBody;
    HeadersContainer mHeaders;
};

#endif // HttpObject_H
