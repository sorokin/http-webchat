#include "httpobject.h"

HttpObject::HttpObject() {}

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

HttpObject::~HttpObject() {}
