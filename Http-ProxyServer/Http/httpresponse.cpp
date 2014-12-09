#include "httpresponse.h"

HttpResponse::HttpResponse() {}

HttpResponse::HttpResponse(int statusCode, const String& reasonPhrase, const String& version,
                           const Data& message) {
    mStatusCode = statusCode;
    mReasonPhrase = reasonPhrase;
    mVersion = version;
    mBody = message;
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

void HttpResponse::setMessageBody(const Data& message) {
    mBody = message;
}

HttpResponse::Data HttpResponse::messageBody() const {
    return mBody;
}

void HttpResponse::setHeader(const String& key, const String& value) {
    mHeaders[key] = value;
}

HttpResponse::String HttpResponse::header(const String& key) {
    if (mHeaders.find(key) != mHeaders.end())
        return mHeaders[key];
    return "";
}

std::map<HttpResponse::String, HttpResponse::String> HttpResponse::headers() const {
    return mHeaders;
}
