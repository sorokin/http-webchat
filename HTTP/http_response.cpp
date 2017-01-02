#include "http_response.h"

HttpResponse::HttpResponse(): HttpMessage() {}

HttpResponse::HttpResponse(Http::Method requestedMethod,
                           const std::string& version, int statusCode, const std::string& reasonPhrase):
        HttpMessage(version), requestedMethod(requestedMethod), statusCode(statusCode), reasonPhrase(reasonPhrase) {}

bool HttpResponse::shouldHaveBody() const {
    return (isParsed && (getHeader("Content-Length") != "" || getHeader("Transfer-Encoding") != ""))
           || (!isParsed && requestedMethod != Http::Method::HEAD);
}

Http::Method HttpResponse::getRequestedMethod() const {
    if (isParsed) {
        throw "Requested method isn't known";
    }
    return requestedMethod;
}

int HttpResponse::getStatusCode() const {
    return statusCode;
}

std::string HttpResponse::getReasonPhrase() const {
    return reasonPhrase;
}

void HttpResponse::setRequestedMethod(Http::Method method) {
    if (!isParsed) {
        throw "Requested method was set during construction";
    }
    requestedMethod = method;
}

bool HttpResponse::append(std::string data) {
    if (!isParsed) {
        throw "The message is constructed, not parsed";
    }

    try {
        switch (state) {
            case START: {
                size_t space = data.find(' ');
                std::string version = data.substr(0, space);
                if (version.size() != 8
                    || version[0] != 'H' || version[1] != 'T' || version[2] != 'T' || version[3] != 'P'
                    || version[4] != '/' || version[6] != '.' || version[5] < '0' || version[5] > '9'
                    || version[7] < '0' || version[7] > '9') {
                    throw "Invalid HTTP version: " + version;
                }
                this->version = version;
                data.erase(0, space + 1);

                space = data.find(' ');
                statusCode = std::stoi(data.substr(0, space));
                data.erase(0, space + 1);

                reasonPhrase = data;
                state = HEADER;
                return false;
            } case HEADER: {
                parseHeader(data);
                return state == FINISHED;
            } case BODY: {
                appendBody(data);
                if (getBodySize() == getDeclaredBodySize()) {
                    state = FINISHED;
                    return true;
                } else {
                    return false;
                }
            } default: {
                throw "The message is finished";
            }
        }
    } catch (...) {
        state = INVALID;
        return true;
    }
}

std::string HttpResponse::finish() {
    if (isParsed && state != FINISHED || !isParsed && state != START) {
        return "";
    }
    if (!isParsed && shouldHaveBody()) {
        setHeader("Content-Length", std::to_string(body.size()));
    }
    state = FINISHED;

    std::string representation = version + " " + std::to_string(statusCode) + " " + reasonPhrase + CRLF;
    for (HeaderMap::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        representation += it->first + ": " + it->second + CRLF;
    }
    representation += CRLF;
    if (shouldHaveBody()) {
        representation += body;
    }

    return representation;
}
