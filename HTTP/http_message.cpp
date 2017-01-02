#include "http_message.h"

HttpMessage::HttpMessage() {
    isParsed = true;
}

HttpMessage::HttpMessage(const std::string& version): version(version) {
    isParsed = false;
}

void HttpMessage::parseHeader(const std::string& header) {
    if (!isParsed) {
        throw "A header can only be parsed in a parsed message";
    }

    if (header.empty()) {
        state = shouldHaveBody() ? BODY : FINISHED;
        return;
    }

    size_t colon = header.find(':');
    if (colon == std::string::npos) {
        throw "Invalid header: there is no colon";
    } else if (colon == 0) {
        throw "Invalid header: empty name";
    }
    std::string name = header.substr(0, colon);
    if (name.find(' ') != std::string::npos) {
        throw "Invalid header: there is a space in the name";
    }

    std::string value = header.substr(colon + 1);
    size_t fns;
    for (fns = 0; fns < value.size() && (value[fns] == ' ' || value[fns] == '\t'); ++fns);
    if (fns == value.size()) {
        throw "Invalid header: empty value";
    }
    size_t lns;
    for (lns = value.size() - 1; value[lns] == ' ' || value[lns] == '\t'; --lns);
    value = value.substr(fns, lns - fns + 1);

    setHeader(name, value);
}

std::string HttpMessage::getVersion() const {
    return version;
}

const HttpMessage::HeaderMap& HttpMessage::getHeaders() const {
    return headers;
}

std::string HttpMessage::getHeader(const std::string& name) const {
    try {
        return headers.at(toLowerCase(name));
    } catch (std::out_of_range oor) {
        return "";
    }
}

std::string HttpMessage::getBody() const {
    return body;
}

size_t HttpMessage::getBodySize() const {
    return body.size();
}

size_t HttpMessage::getDeclaredBodySize() const {
    if (isParsed) {
        if (state == BODY || state == FINISHED) {
            std::string resultAsString;
            if (isChunked) {
                resultAsString = getHeader("transfer-encoding");
            } else {
                resultAsString = getHeader("content-length");
            }
            if (resultAsString == "") {
                throw "The body length wasn't initialized";
            } else {
                return std::stoul(resultAsString);
            }
        } else {
            throw "The message headers aren't finished receiving";
        }
    } else {
        throw "The body length will be set after finishing the message";
    }
}

bool HttpMessage::shouldKeepAlive() const {
    return toLowerCase(getHeader("Connection")) == "keep-alive";
}

void HttpMessage::setHeader(const std::string& name, const std::string& value) {
    headers[toLowerCase(name)] = value;
}

void HttpMessage::appendBody(const std::string& data) {
    if (!shouldHaveBody()) {
        throw "The message shouldn't have a body";
    }

    body += data;
}

HttpMessage::State HttpMessage::getState() const {
    return state;
}

std::string HttpMessage::uriEncode(const std::string& toEncode) {
    std::string result = toEncode;
    return result;
}

std::string HttpMessage::uriDecode(const std::string& toDecode) {
    std::string result = toDecode;
    return result;
}
