#ifndef HTTPWEBCHAT_HTTPMESSAGE_H
#define HTTPWEBCHAT_HTTPMESSAGE_H


#include <map>

#include "http_common.h"

static const std::string CRLF = "\r\n";

class HttpMessage {
public:
    enum State {START, HEADER, BODY, FINISHED, INVALID};

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

    virtual bool shouldHaveBody() const = 0;
    void parseHeader(const std::string&);
public:
    std::string getVersion() const;
    const HeaderMap& getHeaders() const;
    std::string getHeader(const std::string&) const;
    std::string getBody() const;
    size_t getBodySize() const;

    void setHeader(const std::string&, const std::string&);
    void appendBody(const std::string&);

    State getState() const;
    virtual std::string firstLine() const = 0;
    void finish();
    std::string to_string() const;

    size_t getDeclaredBodySize() const;
    bool shouldKeepAlive() const;
};


#endif //HTTPWEBCHAT_HTTPMESSAGE_H
