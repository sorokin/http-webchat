#ifndef ROUTEMATCHER_H
#define ROUTEMATCHER_H
#include <cstring>
#include <Http/httprequest.h>
#include <Http/httputils.h>

class RouteMatcher
{
public:
    RouteMatcher(const std::string& mMethod = "", const std::string& mUrl = "");
    bool match(const HttpRequest& request);
    std::string method();
    std::string url();
private:
    std::string mMethod;
    std::string mUrl;

    void normalize();
};

#endif // ROUTEHANDLER_H
