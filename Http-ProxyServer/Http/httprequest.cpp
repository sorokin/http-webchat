#include "httprequest.h"
#include <QUrl>

void HttpRequest::parseFirstLine(const String &line) {
    std::stringstream in(line);
    in >> mMethod >> mPath >> mVersion;
    mVersion.erase(0, 5);
}

HttpRequest::HttpRequest(CreationMode mode):HttpObject(mode) {}

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
    String h;
    splitUrl(url, h, mPath);
    mHeaders["Host"] = h;
}

HttpRequest::HttpRequest(const String& method, const String& url,
                         const Data& body, const String& version):HttpObject(body, version) {
    mMethod = method;
    String h;
    splitUrl(url, h, mPath);
    mHeaders["Host"] = h;
}

void HttpRequest::setUrl(const String& url) {
    String h;
    splitUrl(url, h, mPath);
    mHeaders["Host"] = h;
}

HttpObject::String HttpRequest::url() const {
    return host() + path();
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

HttpRequest::Data HttpRequest::toString() const {
    HttpRequest::String msg = method() + " " + path() +
            " HTTP/" + version() + END_LINE;
    return msg + HttpObject::toString();
}

HttpRequest::~HttpRequest() {}

