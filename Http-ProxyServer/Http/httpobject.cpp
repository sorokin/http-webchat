#include "httpobject.h"

#include <iostream>
using namespace std;

bool HttpObject::append(const Data& data) {
    //cerr << "append\n";
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
        //cerr << "pos = " << position << " tmp " << tmp.size() << endl;
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
                String key = trim(line.substr(0, pos));
                String value = trim(line.substr(pos + 1, (int)line.size() - pos - 1));
                mHeaders[key] = value;
            }
        }
    }
    return true;
}

void HttpObject::commit() {
    mode = Static;
}

bool HttpObject::isBody() {
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
}

void HttpObject::setVersion(const String& vers) {
    mVersion = vers;
}

HttpObject::String HttpObject::version() const {
    return mVersion;
}

void HttpObject::setHeader(const String& key, const String& val) {
    mHeaders[key] = val;
}

void HttpObject::setHeaders(const std::vector <std::pair <String, String> >& headers) {
    for (int i = 0; i < (int)headers.size(); ++i)
        mHeaders[headers[i].first] = headers[i].second;
}

HttpObject::String HttpObject::header(const String& key) {
    if (mHeaders.find(key) != mHeaders.end())
        return mHeaders[key];
    return "";
}

HttpObject::HeadersContainer HttpObject::headers() const {
    return mHeaders;
}

void HttpObject::setBody(const String& message) {
    mBody = message.c_str();
}

void HttpObject::setBody(const char* message) {
    mBody = message;
}

HttpObject::Data HttpObject::body() const {
    return mBody;
}


HttpObject::String HttpObject::host() const {
    if (mHeaders.find("Host") != mHeaders.end())
        return mHeaders.at("Host");
    return "";
}

int HttpObject::contentLength() const {
    if (mHeaders.find("Content-Length") != mHeaders.end())
        return std::stoi(mHeaders.at("Content-Length"));
    return 0;
}

HttpObject::~HttpObject() {}

