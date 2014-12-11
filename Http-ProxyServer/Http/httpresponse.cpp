#include "httpresponse.h"

HttpResponse::HttpResponse() {}

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

HttpResponse::~HttpResponse() {}
