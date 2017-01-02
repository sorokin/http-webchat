#ifndef HTTPWEBCHAT_HTTPREQUEST_H
#define HTTPWEBCHAT_HTTPREQUEST_H


#include "http_message.h"

class HttpRequest: public HttpMessage {
    Http::Method method;
    std::string uri;

    virtual bool shouldHaveBody() const;
public:
    HttpRequest();
    HttpRequest(Http::Method, const std::string&, const std::string&);

    Http::Method getMethod() const;
    std::string getMethodAsString() const;
    std::string getUri() const;
    std::string getUriEncoded() const;

    void append(std::string);
    virtual std::string finish();
};


#endif //HTTPWEBCHAT_HTTPREQUEST_H
