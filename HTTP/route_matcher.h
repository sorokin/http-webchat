#ifndef HTTPWEBCHAT_ROUTEMATCHER_H
#define HTTPWEBCHAT_ROUTEMATCHER_H


#include "http_request.h"

class RouteMatcher {
    Http::Method method;
    std::string uri;

    static std::string transformRoute(const std::string&);
public:
    RouteMatcher(Http::Method, const std::string&);

    Http::Method getMethod() const;
    std::string getMethodAsString() const;
    std::string getUri() const;

    bool match(const HttpRequest&) const;
};


#endif //HTTPWEBCHAT_ROUTEMATCHER_H
