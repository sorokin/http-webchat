#ifndef HTTPWEBCHAT_HTTPREQUEST_H
#define HTTPWEBCHAT_HTTPREQUEST_H


#include "http_message.h"

class HttpRequest: public HttpMessage {
    Method method;
    std::string uri;

    virtual bool shouldHaveBody();
public:
    HttpRequest();
    HttpRequest(Method, const std::string&, const std::string&);

    Method getMethod();
    std::string getUri();
    std::string getUriEncoded();

    bool append(std::string);
    virtual std::string finish();
};


#endif //HTTPWEBCHAT_HTTPREQUEST_H
