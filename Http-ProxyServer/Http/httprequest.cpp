#include "httprequest.h"

HttpRequest::HttpRequest() {}

HttpRequest::String HttpRequest::getMethod(Method method) {
    int methods[] = {HEAD, GET, PUT, POST, DELETE};
    const char* methodsString[] = {"HEAD", "GET", "PUT", "POST", "DELETE"};
    for (int i = 0; i < 5; ++i)
        if (method == methods[i])
            return methodsString[i];
    return "";
}

HttpRequest::HttpRequest(Method method, const String& url, const char* message, const String& version) {
    mMethod = getMethod(method);
    mUrl = url;
    mVersion = version;
    mBody = message;

}

HttpRequest::HttpRequest(const String& method, const String& url, const char* body, const String& version) {
    mMethod = method;
    mUrl = url;
    mVersion = version;
    mBody = body;
}

void HttpRequest::setMethod(const String& method) {
    mMethod = method;
}


void HttpRequest::setMethod(Method method) {
    mMethod = getMethod(method);
}

HttpRequest::String HttpRequest::method() const {
    return mMethod;
}

void HttpRequest::setUrl(const String& url) {
    mUrl = url;
}

HttpRequest::String HttpRequest::url() const {
    return mUrl;
}

void HttpRequest::setVersion(const String& vers) {
    mVersion = vers;
}

HttpRequest::String HttpRequest::version() const {
    return mVersion;
}

void HttpRequest::setHeader(const String& key, const String& val) {
    mHeaders[key] = val;
}

void HttpRequest::setHeaders(const std::vector <std::pair <String, String> >& headers) {
    for (int i = 0; i < (int)headers.size(); ++i)
        mHeaders[headers[i].first] = headers[i].second;
}

HttpRequest::String HttpRequest::header(const String& key) {
    if (mHeaders.find(key) != mHeaders.end())
        return mHeaders[key];
    return "";
}

std::map <HttpRequest::String, HttpRequest::String> HttpRequest::headers() const {
    return mHeaders;
}

void HttpRequest::setMessageBody(const String& message) {
    mBody = message.c_str();
}

void HttpRequest::setMessageBody(const char* message) {
    mBody = message;
}

const char* HttpRequest::messageBody()  const{
    return mBody;
}
