#include "route_matcher.h"

RouteMatcher::RouteMatcher(Http::Method method, const std::string& uri):
        method(method), uri(transformRoute(uri)) {}

std::string RouteMatcher::transformRoute(const std::string& uri) {
    if (uri == "*") {
        return uri;
    }

    std::string result = toLowerCase(uri);
    if (result[result.size() - 1] != '/' && result.size() != 1) {
        result.erase(result.size() - 1, 1);
    }
    return result;
}

Http::Method RouteMatcher::getMethod() const {
    return method;
}

std::string RouteMatcher::getMethodAsString() const {
    return Http::methodToString(method);
}

std::string RouteMatcher::getUri() const {
    return uri;
}

bool RouteMatcher::match(const HttpRequest& request) const {
    std::string requestUri = request.getUri();
    size_t symbol = requestUri.find("//");
    if (symbol != std::string::npos) {
        requestUri.erase(0, symbol + 2);
    }
    symbol = requestUri.find('/');
    if (symbol != std::string::npos && symbol != 0) {
        requestUri.erase(0, symbol);
    }
    symbol = requestUri.find('?');
    if (symbol != std::string::npos) {
        requestUri.erase(symbol, requestUri.size() - symbol);
    }

    return method == request.getMethod() && uri == requestUri;
}
