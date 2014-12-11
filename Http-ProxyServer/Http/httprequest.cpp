#include "httprequest.h"
#include <QUrl>

HttpRequest::HttpRequest() {}

HttpRequest::String HttpRequest::getMethod(Method method) {
    int methods[] = {HEAD, GET, PUT, POST, DELETE};
    const char* methodsString[] = {"HEAD", "GET", "PUT", "POST", "DELETE"};
    for (int i = 0; i < 5; ++i)
        if (method == methods[i])
            return methodsString[i];
    return "";
}

HttpRequest::HttpRequest(Method method, const String& url, const Data& body,
                         const String& version):HttpObject(body, version) {
    mMethod = getMethod(method);
    splitUrl(url, mHost, mPath);
    mHeaders["Host"] = mHost;
}

HttpRequest::HttpRequest(const String& method, const String& url,
                         const Data& body, const String& version):HttpObject(body, version) {
    mMethod = method;
    splitUrl(url, mHost, mPath);
    mHeaders["Host"] = mHost;
}

void HttpRequest::setUrl(const String& url) {
    splitUrl(url, mHost, mPath);
    mHeaders["Host"] = mHost;
}

HttpObject::String HttpRequest::url() const {
    return mHost + mPath;
}

HttpRequest::String HttpRequest::host() const {
    return mHost;
}

HttpRequest::String HttpRequest::path() const {
    return mPath;
}

void HttpRequest::splitUrl(String url, String& host, String& path) {
    if (!QString(url.c_str()).startsWith("http://", Qt::CaseInsensitive))
        url = "http://" + url;
    QUrl t(url.c_str());
    host = t.host().toStdString();
    path = t.path().toStdString();
    if (path == "")
        path = "/";
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

HttpRequest::~HttpRequest() {}

