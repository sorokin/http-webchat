#ifndef HttpObject_H
#define HttpObject_H
#include <cstring>
#include <string>
#include <map>
#include <vector>
#include <sstream>

class HttpObject
{
public:
    typedef std::string Data;
    typedef std::string String;
    typedef std::map <String, String> HeadersContainer;

    //Dynamic creating of object
    enum CreationMode {Dynamic, Static};
    bool append(const Data& data);
    void commit();
    bool isBody();
    CreationMode creationMode();

    HttpObject(CreationMode mode);
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

    //get of headers
    String host() const;
    int contentLength() const;
protected:
    String mVersion;
    Data mBody;
    HeadersContainer mHeaders;
    virtual void parseFirstLine(const String& line) = 0;
private:
    String trim(const String& s);

    bool mIsBody;
    Data tmp;
    CreationMode mode;
    int position;
};

#endif // HttpObject_H
