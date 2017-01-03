#include <iostream>
#include "http_common.h"

std::string Http::methodToString(Http::Method method) {
    switch (method) {
        case GET:
            return "GET";
        case HEAD:
            return "HEAD";
        case OPTIONS:
            return "OPTIONS";
        case POST:
            return "POST";
        default:
            throw std::runtime_error("Invalid HTTP method");
    }
}

Http::Method Http::stringToMethod(const std::string& string) {
    if (string == "GET") {
        return GET;
    } else if (string == "HEAD") {
        return HEAD;
    } else if (string == "OPTIONS") {
        return OPTIONS;
    } else if (string == "POST") {
        return POST;
    } else {
        throw std::runtime_error("Invalid HTTP method: " + string);
    }
}

std::string Http::uriEncode(const std::string& toEncode) {
    std::string result = "";
    for (std::string::const_iterator it = toEncode.begin(); it != toEncode.end(); ++it) {
        switch (*it) {
            case '\t': {
                result += "%09";
            } case '\n': {
                result += "%0A";
            } case '\r': {
                result += "%0D";
            } case ' ': {
                result += "%20";
            } case '!': {
                result += "%21";
            } case '\"': {
                result += "%22";
            } case '#': {
                result += "%23";
            } case '$': {
                result += "%24";
            } case '%': {
                result += "%25";
            } case '&': {
                result += "%26";
            } case '\'': {
                result += "%27";
            } case '(': {
                result += "%28";
            } case ')': {
                result += "%29";
            } case '*': {
                result += "%2A";
            } case '+': {
                result += "%2B";
            } case ',': {
                result += "%2C";
            } case '-': {
                result += "%2D";
            } case '.': {
                result += "%2E";
            } case '/': {
                result += "%2F";
            } case ':': {
                result += "%3A";
            } case ';': {
                result += "%3B";
            } case '<': {
                result += "%3C";
            } case '=': {
                result += "%3D";
            } case '>': {
                result += "%3E";
            } case '?': {
                result += "%3F";
            } case '@': {
                result += "%40";
            } case '[': {
                result += "%5B";
            } case '\\': {
                result += "%5C";
            } case ']': {
                result += "%5D";
            } case '^': {
                result += "%5E";
            } case '_': {
                result += "%5F";
            } case '`': {
                result += "%60";
            } case '{': {
                result += "%7B";
            } case '|': {
                result += "%7C";
            } case '}': {
                result += "%7D";
            } case '~': {
                result += "%7E";
            } default: {
                result += *it;
            }
        }
    }
    return result;
}

std::string Http::uriDecode(const std::string& toDecode) {
    std::string result = "";
    size_t faps = 0;
    for (size_t cur = 0; cur < toDecode.size(); ++cur) {
        if (toDecode[cur] == '%') {
            if (cur > toDecode.size() - 3) {
                throw std::runtime_error("Invalid URL-encoded string (percent encoding near the end): " + toDecode);
            }
            if (faps < cur) {
                result += toDecode.substr(faps, cur - faps);
            }

            char fst = toDecode[cur + 1];
            char snd = toDecode[cur + 2];
            if (fst == '0' && snd == '9') {
                result += '\t';
            } else if (fst == '0' && snd == 'A') {
                result += '\n';
            } else if (fst == '0' && snd == 'D') {
                result += '\r';
            } else if (fst == '2' && snd == '0') {
                result += ' ';
            } else if (fst == '2' && snd == '1') {
                result += '!';
            } else if (fst == '2' && snd == '2') {
                result += '\"';
            } else if (fst == '2' && snd == '3') {
                result += '#';
            } else if (fst == '2' && snd == '4') {
                result += '$';
            } else if (fst == '2' && snd == '5') {
                result += '%';
            } else if (fst == '2' && snd == '6') {
                result += '&';
            } else if (fst == '2' && snd == '7') {
                result += '\'';
            } else if (fst == '2' && snd == '8') {
                result += '(';
            } else if (fst == '2' && snd == '9') {
                result += ')';
            } else if (fst == '2' && snd == 'A') {
                result += '*';
            } else if (fst == '2' && snd == 'B') {
                result += '+';
            } else if (fst == '2' && snd == 'C') {
                result += ',';
            } else if (fst == '2' && snd == 'D') {
                result += '-';
            } else if (fst == '2' && snd == 'E') {
                result += '.';
            } else if (fst == '2' && snd == 'F') {
                result += '/';
            } else if (fst == '3' && snd == 'A') {
                result += ':';
            } else if (fst == '3' && snd == 'B') {
                result += ';';
            } else if (fst == '3' && snd == 'C') {
                result += '<';
            } else if (fst == '3' && snd == 'D') {
                result += '=';
            } else if (fst == '3' && snd == 'E') {
                result += '>';
            } else if (fst == '3' && snd == 'F') {
                result += '?';
            } else if (fst == '4' && snd == '0') {
                result += '@';
            } else if (fst == '5' && snd == 'B') {
                result += '[';
            } else if (fst == '5' && snd == 'C') {
                result += '\\';
            } else if (fst == '5' && snd == 'D') {
                result += ']';
            } else if (fst == '5' && snd == 'E') {
                result += '^';
            } else if (fst == '5' && snd == 'F') {
                result += '_';
            } else if (fst == '6' && snd == '0') {
                result += '`';
            } else if (fst == '7' && snd == 'B') {
                result += '{';
            } else if (fst == '7' && snd == 'C') {
                result += '|';
            } else if (fst == '7' && snd == 'D') {
                result += '}';
            } else if (fst == '7' && snd == 'E') {
                result += '~';
            } else {
                throw std::runtime_error("Invalid URL-encoded string (invalid percent encoding "
                                         + toDecode.substr(cur, 3) + "): " + toDecode);
            }
            faps = cur + 3;
        }
    }
    if (faps < toDecode.size()) {
        result += toDecode.substr(faps, toDecode.size() - faps);
    }

    return result;
}

std::string Http::getUriPath(const std::string& uri) {
    std::string path = uri;
    size_t authority = path.find("//");
    if (authority != std::string::npos) {
        path.erase(0, authority + 2);
    }

    size_t slash = path.find('/');
    if (slash == std::string::npos) {
        return "/";
    } else {
        path.erase(0, slash);

        size_t question = path.find('?');
        if (question != std::string::npos) {
            return path.substr(0, question);
        } else {
            size_t hash = path.find('#');
            if (hash != std::string::npos) {
                path.erase(hash, path.size() - hash);
            }
            if (path.size() > 1 && path[path.size() - 1] == '/') {
                path.erase(path.size() - 1, 1);
            }

            return path;
        }
    }
}

std::map<std::string, std::string> Http::queryParameters(const std::string& uri) {
    size_t question = uri.find('?');
    if (question == std::string::npos) {
        return std::map<std::string, std::string>();
    }

    std::map<std::string, std::string> parameters;
    std::string query = uri.substr(question + 1, uri.size() - question - 1);
    size_t ampersand;
    size_t hash;
    while ((ampersand = query.find('&')) != std::string::npos
           && ((hash = query.find('#')) == std::string::npos || ampersand < hash)) {
        std::string parameter = query.substr(0, ampersand);
        query.erase(0, ampersand + 1);

        size_t equal = parameter.find('=');
        if (equal == std::string::npos || equal == 0 || equal == parameter.size() - 1) {
            throw std::runtime_error("Wrong query parameter: " + parameter);
        }
        std::string value = parameter.substr(equal + 1, parameter.size() - equal - 1);
        if (value.find('=') != std::string::npos) {
            throw std::runtime_error("Wrong query parameter: " + parameter);
        }
        parameters[uriDecode(parameter.substr(0, equal))] = uriDecode(value);
    }

    if (hash != std::string::npos) {
        query = query.substr(0, hash);
    }
    size_t equal = query.find('=');
    if (equal == std::string::npos || equal == 0 || equal == query.size() - 1) {
        throw std::runtime_error("Wrong query parameter: " + query);
    }
    std::string value = query.substr(equal + 1, query.size() - equal - 1);
    if (value.find('=') != std::string::npos) {
        throw std::runtime_error("Wrong query parameter: " + query);
    }
    parameters[uriDecode(query.substr(0, equal))] = uriDecode(value);

    return parameters;
}
