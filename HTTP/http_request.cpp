#include "http_request.h"

HttpRequest::HttpRequest(): HttpMessage() {}

HttpRequest::HttpRequest(HttpMessage::Method method, const std::string& uri, const std::string& version):
        HttpMessage(version), method(method), uri(uri) {}

bool HttpRequest::shouldHaveBody() {
    return (isParsed && (getHeader("Content-Length") != NULL || getHeader("Transfer-Encoding") != NULL))
           || (!isParsed && method == POST);
}

HttpMessage::Method HttpRequest::getMethod() {
    return method;
}

std::string HttpRequest::getUri() {
    return uri;
}

std::string HttpRequest::getUriEncoded() {
    return uriEncode(uri);
}

bool HttpRequest::append(std::string data) {
    if (!isParsed) {
        throw "The message is constructed, not parsed";
    }

    switch (state) {
        case START:
            size_t space = data.find(' ');
            std::string method = data.substr(0, space);
            try {
                this->method = stringToMethod(method);
            } catch (std::string error) {
                state = INVALID;
                return true;
            }
            data.erase(0, space + 1);

            space = data.find(' ');
            uri = uriDecode(data.substr(0, space));
            data.erase(0, space + 1);

            if (data.size() != 7
                || data[0] != 'H' || data[1] != 'T' || data[2] != 'T' || data[3] != 'P' || data[5] != '.'
                || data[4] < '0' || data[4] > '9' || data[6] < '0' || data[6] > '9') {
                state = INVALID;
                return true;
            } else {
                version = data;
                state = HEADER;
                return false;
            }
        case HEADER:
            try {
                parseHeader(data);
                return state == FINISHED;
            } catch (std::string error) {
                state = INVALID;
                return true;
            }
        case BODY:
            appendBody(data);
            if (getBodySize() == declaredBodySize()) {
                state = FINISHED;
                return true;
            } else {
                return false;
            }
        default:
            throw "The message is finished";
    }
}

std::string HttpRequest::finish() {
    if (!isParsed && shouldHaveBody()) {
        setHeader("Content-Length", std::to_string(body.size()));
    }
    if (isParsed && state != FINISHED || !isParsed && state != START) {
        return NULL;
    }
    state = FINISHED;

    std::string representation = methodToString(method) + " " + uriEncode(uri) + " " + version + CRLF;
    for (HeaderMap::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        representation += it->first + ": " + it->second + CRLF;
    }
    representation += CRLF;
    if (shouldHaveBody()) {
        representation += body;
    }

    return representation;
}
