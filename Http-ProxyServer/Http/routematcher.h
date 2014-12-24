#ifndef ROUTEMATCHER_H
#define ROUTEMATCHER_H
#include <cstring>
#include <Http/httprequest.h>
#include <Http/httputils.h>

class RouteMatcher
{
public:
    RouteMatcher(const std::string& method = "", const std::string& url = "");
    RouteMatcher(const std::string& url);
    bool match(const HttpRequest& request);
private:
    std::string method;
    std::string url;

    void normalize();
};

#endif // ROUTEHANDLER_H
