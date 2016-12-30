#ifndef HttpObject_H
#define HttpObject_H

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

class HttpObject
{
public:
    typedef std::string Data;
    typedef std::string String;
    typedef std::vector <std::pair <String, String> > HeadersContainer;

    const String END_LINE = "\n";

    //Dynamic creating of object
    enum CreationMode {Dynamic, Static};
    bool append(const Data& data);
    void commit();
    bool hasBody();
//    CreationMode creationMode();

    HttpObject(CreationMode mode);
    HttpObject(const Data& body, const String& version);
//    virtual void setUrl(const String& url) = 0;
    virtual String url() const = 0;
//    void setVersion(const String& vers);
    String version() const;
    void addHeader(const String& key, const String& val);
//    void addHeaders(const std::vector <std::pair <String, String> >& headers);
    void setHeader(const String& key, const String& val);
    String header(const String& key);
//    HeadersContainer headers() const;
    void setBody(const Data& message);
//    void setBody(const char* message);
    virtual String toString() const;
    Data body() const;
    static std::map<std::string, std::string> parseForm(std::string raw);
    bool notKeptAlive() const;
    std::string findHeader(std::string key) const;

    String host() const;
    int contentLength() const;

    virtual ~HttpObject();
protected:
    String mVersion;
    Data mBody;
    HeadersContainer mHeaders;
    virtual void parseFirstLine(const String& line) = 0;
private:
    std::string toLower(std::string s) const;
    String trim(const String& s);

    bool mIsBody;
    Data tmp;
    CreationMode mode;
    size_t position;
};

#endif // HttpObject_H