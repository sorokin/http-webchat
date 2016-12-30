#include "httpobject.h"

using namespace std;

bool HttpObject::append(const Data& data) {
    if (mode == Static)
        return false;

    if (mIsBody)
        mBody += data;
    else {
        tmp += data;
        for (; position < tmp.size(); ++position)
            if (tmp[position] == '\n' &&
                    ((position >= 1 && tmp[position - 1] == '\n') ||
                            (position >= 2 && tmp[position - 1] == '\r' && tmp[position - 2] == '\n')))
                break;
        if (position != tmp.size()) {
            mIsBody = true;
            if (position + 1 < tmp.size()) {
                mBody = tmp.substr(position + 1, tmp.size() - position - 1);
                tmp.erase(position + 1, tmp.size() - position - 1);
            }

            std::stringstream in(tmp);
            //cerr << "s = " << tmp << endl;
            std::string line;
            getline(in, line);
            parseFirstLine(line);
            while (getline(in, line)) {
                size_t pos = line.find(":");
                if (pos == string::npos)
                    continue;
                String key = toLower(trim(line.substr(0, pos)));
                String value = trim(line.substr(pos + 1, line.size() - pos - 1));
                mHeaders.push_back(make_pair(key, value));
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
    size_t i = 0, j = s.size();
    while (i < s.size() && s[i] == ' ' ) ++i;
    while (j > 0 && s[j - 1] == ' ') --j;
    if (i >= j)
        return "";
    return s.substr(i, j - i);
}

//HttpObject::CreationMode HttpObject::creationMode() {
//    return mode;
//}

HttpObject::HttpObject(HttpObject::CreationMode mode):mIsBody(false), mode(mode), position(0) {}

HttpObject::HttpObject(const Data& body, const String& version) {
    mBody = body;
    mVersion = version;
    setHeader("Content-Length", std::to_string(body.size()));
}

//void HttpObject::setVersion(const String& vers) {
//    mVersion = vers;
//}

HttpObject::String HttpObject::version() const {
    return mVersion;
}

void HttpObject::addHeader(const String& key, const String& val) {
    mHeaders.push_back(make_pair(toLower(key), toLower(val)));
}

void HttpObject::setHeader(const String& key, const String& val) {
    String k = toLower(key);
    for (size_t i = 0; i < mHeaders.size(); ++i)
        if (mHeaders[i].first == k) {
            mHeaders[i].second = val;
            return;
        }
    mHeaders.push_back(make_pair(toLower(key), toLower(val)));
}

//void HttpObject::addHeaders(const std::vector <std::pair <String, String> >& headers) {
//    for (int i = 0; i < (int)headers.size(); ++i)
//        mHeaders.push_back(make_pair(toLower(headers[i].first), headers[i].second));
//}

HttpObject::String HttpObject::header(const String& key) {
    String k = toLower(key);
    for (size_t i = 0; i < mHeaders.size(); ++i)
        if (mHeaders[i].first == k)
            return mHeaders[i].second;
    return "";
}

//HttpObject::HeadersContainer HttpObject::headers() const {
//    return mHeaders;
//}

void HttpObject::setBody(const Data& body) {
    mBody = body.c_str();
    setHeader("Content-Length", std::to_string(body.size()));
}

//void HttpObject::setBody(const char* message) {
//    mBody = message;
//}

HttpObject::Data HttpObject::body() const {
    return mBody;
}

std::map<std::string, std::string> HttpObject::parseForm(std::string raw) {
    map<string, string> form;
    string name = "", value = "";
    bool isInName = true;
    for (size_t i = 0; i < raw.length(); ++i) {
        if (raw[i] == '=') {
            if (isInName) {
                isInName = false;
            } else {
                return map<string, string>();
            }
        } else if (raw[i] == '&') {
            if (!isInName) {
                isInName = true;
                if (form.find(name) != form.end()) {
                    return map<string, string>();
                }
                form[name] = value;
                name = "";
                value = "";
            }
        } else {
            if (isInName) {
                name += raw[i];
            } else {
                value += raw[i];
            }
        }
    }
    if (!isInName) {
        if (form.find(name) != form.end()) {
            return map<string, string>();
        }
        form[name] = value;
        return form;
    } else {
        return map<string, string>();
    }
}

std::string HttpObject::findHeader(std::string key) const{
    for (size_t i = 0; i < mHeaders.size(); ++i)
        if (mHeaders[i].first == key)
            return mHeaders[i].second;
    return "";
}

HttpObject::String HttpObject::host() const {
    return toLower(findHeader("host"));
}

int HttpObject::contentLength() const {
    std::string cleng = findHeader("content-length");
    if (cleng.size() == 0)
        return 0;
    return atoi(cleng.c_str());
}

bool HttpObject::notKeptAlive() const {
    //cerr << "find keep alive = " << toLower(findHeader("connection")) << "#" << endl;
    //cerr << "bool = " << (toLower(findHeader("connection")) == "keep-alive") << endl;
    return toLower(findHeader("connection")) != "keep-alive";
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

std::string HttpObject::toLower(std::string s) const {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}
