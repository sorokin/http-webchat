#include "httpresponse.h"

void HttpResponse::parseFirstLine(const String &line) {
    std::stringstream in(line);
    in >> mVersion >> mStatusCode >> mReasonPhrase;
    mVersion.erase(0, 5);
}

HttpResponse::HttpResponse(CreationMode mode):HttpObject(mode) {}

HttpResponse::HttpResponse(int statusCode, const String& reasonPhrase, const String& version,
                           const Data& body):HttpObject(body, version) {
    mStatusCode = statusCode;
    mReasonPhrase = reasonPhrase;
}

void HttpResponse::setUrl(const String& url) {
    mUrl = url;
}

HttpResponse::String HttpResponse::url() const {
    return mUrl;
}

void HttpResponse::setStatusCode(int statusCode) {
    mStatusCode = statusCode;
}

int HttpResponse::statusCode() const {
    return mStatusCode;
}

void HttpResponse::setReasonPhrase(const String& reasonPhrase) {
    mReasonPhrase = reasonPhrase;
}

HttpResponse::String HttpResponse::reasonPhrase() const {
    return mReasonPhrase;
}

HttpResponse::String HttpResponse::toString() const {
    String msg = "HTTP/" + version() + " ";
    char s[5];
    sprintf(s, "%d", mStatusCode);
    msg += String(s) + " " + mReasonPhrase + END_LINE + HttpObject::toString();
    return msg;
}

HttpResponse::~HttpResponse() {}
