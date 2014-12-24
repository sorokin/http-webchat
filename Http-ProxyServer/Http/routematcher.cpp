#include "routematcher.h"

RouteMatcher::RouteMatcher(const std::string& method, const std::string& url):method(method), url(url)
{
    normalize();
}

RouteMatcher::RouteMatcher(const std::string& url):method(""), url(url)
{
    normalize();
}

bool RouteMatcher::match(const HttpRequest& request) {
    std::string m = HttpUtils::toLower(request.method());
    std::string r = HttpUtils::transformRoute(request.path());
    return m == method && r == url;
}

void RouteMatcher::normalize() {
    method = HttpUtils::toLower(method);
    url = HttpUtils::transformRoute(url);
}

