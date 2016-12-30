#include "routematcher.h"

RouteMatcher::RouteMatcher(const std::string& method, const std::string& url):mMethod(method), mUrl(url)
{
    normalize();
}

bool RouteMatcher::match(const HttpRequest& request) {
    std::string m = HttpUtils::toLower(request.method());
    std::string r = HttpUtils::transformRoute(request.path());
    return (mMethod == "*" || mMethod == m) && (mUrl == "*" || mUrl == r);
}

void RouteMatcher::normalize() {
    mMethod = HttpUtils::toLower(mMethod);
    mUrl = HttpUtils::transformRoute(mUrl);
}

std::string RouteMatcher::method() {
    return mMethod;
}

std::string RouteMatcher::url() {
    return mUrl;
}
