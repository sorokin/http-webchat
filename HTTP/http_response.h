#ifndef HTTPWEBCHAT_HTTPRESPONSE_H
#define HTTPWEBCHAT_HTTPRESPONSE_H


#include "http_message.h"

class HttpResponse: public HttpMessage {
    Http::Method requestedMethod;
    int statusCode;
    std::string reasonPhrase;

    virtual bool shouldHaveBody() const;
public:
    HttpResponse();
    HttpResponse(Http::Method, const std::string&, int, const std::string&);

    Http::Method getRequestedMethod() const;
    int getStatusCode() const;
    std::string getReasonPhrase() const;

    void setRequestedMethod(Http::Method);

    void append(std::string);
    virtual std::string firstLine() const;
};


#endif //HTTPWEBCHAT_HTTPRESPONSE_H
