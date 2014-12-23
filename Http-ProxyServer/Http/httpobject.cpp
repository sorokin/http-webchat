#include "httpobject.h"

#include <iostream>
using namespace std;

bool HttpObject::append(const Data& data) {
    if (mode == Static)
        return false;

    if (mIsBody)
        mBody += data;
    else {
        tmp += data;
        for (;position < tmp.size(); ++position)
            if (tmp[position] == '\n' &&
                    (position >= 1 && tmp[position - 1] == '\n' ||
                     position >= 2 && tmp[position - 1] == '\r' && tmp[position - 2] == '\n'))
                break;
        if (position != tmp.size()) {
            mIsBody = true;
            if (position + 1 < tmp.size()) {
                mBody = tmp.substr(position + 1, (int)tmp.size() - position - 1);
                tmp.erase(position + 1, (int)tmp.size() - position - 1);
            }

            std::stringstream in(tmp);
            //cerr << "s = " << tmp << endl;
            std::string line;
            getline(in, line);
            parseFirstLine(line);
            while (getline(in, line)) {
                int pos = line.find(":");
                if (pos == -1)
                    continue;
                String key = toLower(trim(line.substr(0, pos)));
                String value = toLower(trim(line.substr(pos + 1, (int)line.size() - pos - 1)));
                mHeaders[key] = value;
            }
        }
    }
    return true;
}

void HttpObject::commit() {
    mode = Static;
}

bool HttpObject::hasBody() {
    return mIsBody;
}

HttpObject::String HttpObject::trim(const String& s) {
    int i = 0, j = s.size() - 1;
    while (i < s.size() && s[i] == ' ' ) ++i;
    while (j >= 0 && s[j] == ' ') --j;
    if (i > j)
        return "";
    return s.substr(i, j - i + 1);
}

HttpObject::CreationMode HttpObject::creationMode() {
    return mode;
}

HttpObject::HttpObject(HttpObject::CreationMode mode):mIsBody(false), mode(mode), position(0) {}

HttpObject::HttpObject(const Data& body, const String& version) {
    mBody = body;
    mVersion = version;
    setHeader("Content-Length", std::to_string(body.size()));
}

void HttpObject::setVersion(const String& vers) {
    mVersion = vers;
}

HttpObject::String HttpObject::version() const {
    return mVersion;
}

void HttpObject::setHeader(const String& key, const String& val) {
    mHeaders[toLower(key)] = toLower(val);
}

void HttpObject::setHeaders(const std::vector <std::pair <String, String> >& headers) {
    for (int i = 0; i < (int)headers.size(); ++i)
        mHeaders[toLower(headers[i].first)] = headers[i].second;
}

HttpObject::String HttpObject::header(const String& key) {
    String k = toLower(key);
    if (mHeaders.find(k) != mHeaders.end())
        return mHeaders[k];
    return "";
}

HttpObject::HeadersContainer HttpObject::headers() const {
    return mHeaders;
}

void HttpObject::setBody(const Data& body) {
    mBody = body.c_str();
    setHeader("Content-Length", std::to_string(body.size()));
}

void HttpObject::setBody(const char* message) {
    mBody = message;
}

HttpObject::Data HttpObject::body() const {
    return mBody;
}


HttpObject::String HttpObject::host() const {
    if (mHeaders.find("host") != mHeaders.end())
        return mHeaders.at("host");
    return "";
}

int HttpObject::contentLength() const {
    if (mHeaders.find("content-length") != mHeaders.end())
        return std::stoi(mHeaders.at("content-length"));
    return 0;
}

bool HttpObject::isKeepAlive() const {
    if (mHeaders.find("connection") != mHeaders.end())
        return mHeaders.at("connection") != "keep-alive";
    return false;
}

HttpObject::String HttpObject::toString() const {
    String msg;
    for (HeadersContainer::const_iterator it = mHeaders.begin(); it != mHeaders.end(); it++)
        msg += it->first + ": " + it->second + END_LINE;
    msg += END_LINE;
    msg += mBody;
    return msg;
}

HttpObject::~HttpObject() {}

std::string HttpObject::toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}
