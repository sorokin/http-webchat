#ifndef HTTPWEBCHAT_HTTPMESSAGE_H
#define HTTPWEBCHAT_HTTPMESSAGE_H


#include <map>

#include "../common.h"

class HttpMessage {
public:
    enum Method {GET, HEAD, POST};
    enum State {START, HEADER, BODY, FINISHED, INVALID};

    typedef std::map<std::string, std::string> CookieMap; // TODO: реализовать поддержку куков
    typedef std::map<std::string, std::string> HeaderMap;
protected:
    State state;
    bool isParsed;
    bool hasBody;
    bool isChunked;

    std::string version;
    HeaderMap headers;
    std::string body;

    HttpMessage();
    HttpMessage(const std::string&);

    virtual bool shouldHaveBody() = 0;
    void parseHeader(const std::string&);
public:
    static const std::string CRLF = "\r\n";

    std::string getVersion();
    const HeaderMap& getHeaders();
    std::string getHeader(const std::string&);
    std::string getBody();
    size_t getBodySize();

    void setHeader(const std::string&, const std::string&);
    void appendBody(const std::string&);

    virtual std::string finish() = 0;

    size_t declaredBodySize();

    std::string uriEncode(const std::string&);
    std::string uriDecode(const std::string&);

    std::string methodToString(Method);
    Method stringToMethod(const std::string&);
};


#endif //HTTPWEBCHAT_HTTPMESSAGE_H
