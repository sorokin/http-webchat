#include "http_common.h"

std::string Http::methodToString(Http::Method method) {
    switch (method) {
        case GET:
            return "GET";
        case HEAD:
            return "HEAD";
        case POST:
            return "POST";
        default:
            throw "Invalid HTTP method";
    }
}

Http::Method Http::stringToMethod(const std::string& string) {
    if (string == "GET") {
        return GET;
    } else if (string == "HEAD") {
        return HEAD;
    } else if (string == "POST") {
        return POST;
    } else {
        throw "Invalid HTTP method: " + string;
    }
}
