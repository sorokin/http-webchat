#include "http_response.h"

HttpResponse::HttpResponse(): HttpMessage() {}

HttpResponse::HttpResponse(Method requestedMethod,
                           const std::string& version, int statusCode, const std::string& reasonPhrase):
        HttpMessage(version), requestedMethod(requestedMethod), statusCode(statusCode), reasonPhrase(reasonPhrase) {}

bool HttpResponse::shouldHaveBody() {
    return (isParsed && (getHeader("Content-Length") != NULL || getHeader("Transfer-Encoding") != NULL))
           || (!isParsed && requestedMethod != HEAD);
}

HttpMessage::Method HttpResponse::getRequestedMethod() {
    return requestedMethod;
}

int HttpResponse::getStatusCode() {
    return statusCode;
}

std::string HttpResponse::getReasonPhrase() {
    return reasonPhrase;
}

void HttpResponse::setRequestedMethod(HttpMessage::Method method) {
    if (!isParsed) {
        throw "Requested method was set during construction";
    }
    requestedMethod = method;
}

bool HttpResponse::append(std::string data) {
    if (!isParsed) {
        throw "The message is constructed, not parsed";
    }

    switch (state) {
        case START:
            size_t space = data.find(' ');
            std::string version = data.substr(0, space);
            if (version.size() != 7
                || version[0] != 'H' || version[1] != 'T' || version[2] != 'T' || version[3] != 'P'
                || version[5] != '.' || version[4] < '0' || version[4] > '9' || version[6] < '0' || version[6] > '9') {
                state = INVALID;
                return true;
            } else {
                this->version = version;
            }
            data.erase(0, space + 1);

            space = data.find(' ');
            try {
                statusCode = std::stoi(data.substr(0, space));
            } catch (...) {
                state = INVALID;
                return true;
            }
            data.erase(0, space + 1);

            reasonPhrase = data;
            state = HEADER;
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

std::string HttpResponse::finish() {
    if (!isParsed && shouldHaveBody()) {
        setHeader("Content-Length", std::to_string(body.size()));
    }
    if (isParsed && state != FINISHED || !isParsed && state != START) {
        return NULL;
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
