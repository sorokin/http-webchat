#ifndef HTTPWEBCHAT_HTTPRESPONSE_H
#define HTTPWEBCHAT_HTTPRESPONSE_H


#include "http_message.h"

class HttpResponse: public HttpMessage {
    Method requestedMethod;
    int statusCode;
    std::string reasonPhrase;

    virtual bool shouldHaveBody();
public:
    HttpResponse();
    HttpResponse(Method, const std::string&, int, const std::string&);

    Method getRequestedMethod();
    int getStatusCode();
    std::string getReasonPhrase();

    void setRequestedMethod(Method);

    bool append(std::string);
    virtual std::string finish();
};


#endif //HTTPWEBCHAT_HTTPRESPONSE_H
