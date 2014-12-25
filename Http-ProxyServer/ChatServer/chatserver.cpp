#include "chatserver.h"

ChatServer::ChatServer(Application* app):
    httpServer(new HttpServer(app)), COOKIE_USER("user"), COOKIE_HASH("hash"), numUsers(0)
{
    httpServer->addRouteMatcher(RouteMatcher(), [this](HttpRequest req, HttpServer::Response resp) {
        int i = (int)req.path().size() - 1;
        std::string path = req.path();
        while (i >= 0 && path[i] != '/') --i;
        std::string filename = path.substr(i + 1, path.size() - i - 1);
        if (filename == "")
            filename = "index.html";
        ifstream in(filename.c_str());
        if (!in) {
            HttpResponse r(404, "Not Found", req.version(),
                           "<html><head>"
                           "<title>404 Not Found</title>"
                           "</head><body>"
                           "<h1>Not Found</h1>"
                           "<p>The requested URL " + path + " was not found on this server.</p>"
                           "<hr></body></html>");
            resp.response(r);
            return;

        }

        std::string s, ret;
        while (getline(in, s))
            ret += s + "\n";
        std::string cookie = req.header("Cookie");
        UserType userId = getUserIdByCookie(cookie);
        if (userId == 0)
            userId = ++numUsers;
        HttpResponse r(200, "OK", req.version(), ret);
        if (req.isKeepAlive())
            r.addHeader("Connection", "Keep-Alive");

        int pos = filename.find(".");
        std::string type = filename.substr(pos + 1, filename.size() - pos - 1);

        if (type == "js")
            r.addHeader("Content-Type", "application/javascript");
        else if (type == "html")
            r.addHeader("Content-Type", "text/html");

        r.addHeader("Set-Cookie", "user=" + to_string(userId) + "; expires=Fri, 31 Dec 2099 23:59:59 GMT;");
        r.addHeader("Set-Cookie", "hash=" + to_string(hash(userId)) + "; expires=Fri, 31 Dec 2099 23:59:59 GMT;");
        resp.response(r);
    });

    httpServer->addRouteMatcher(RouteMatcher("POST", "/messages"), [=] (HttpRequest req, HttpServer::Response resp) {
        std::string cookie = req.header("Cookie");
        UserType userId = getUserIdByCookie(cookie);
        if (userId != 0) {
            if (lastReadMessage.find(userId) == lastReadMessage.end()) {
                history.push_back(Message(userId, time(NULL), "user" + to_string(userId) + " come in!"));
                firstReadMessage[userId] = lastReadMessage[userId] = history.size() - 1;
            }

            HttpResponse r(200, "OK", req.version());
            if (req.isKeepAlive())
                r.addHeader("Connection", "Keep-Alive");
            history.push_back(Message(userId, time(NULL), getMessage(req.body())));
            r.addHeader("Set-Cookie", "user=" + to_string(userId) + "; expires=Fri, 31 Dec 2099 23:59:59 GMT;");
            r.addHeader("Set-Cookie", "hash=" + to_string(hash(userId)) + "; expires=Fri, 31 Dec 2099 23:59:59 GMT;");
            resp.response(r);
        } else {
            HttpResponse r(401, "Unauthorized", req.version());
            if (req.isKeepAlive())
                r.addHeader("Connection", "Keep-Alive");
            resp.response(r);
        }
    });

    httpServer->addRouteMatcher(RouteMatcher("GET", "/messages"), [=] (HttpRequest req, HttpServer::Response resp) {
        std::string cookie = req.header("Cookie");
        UserType userId = getUserIdByCookie(cookie);

        if (userId != 0) {
            if (lastReadMessage.find(userId) == lastReadMessage.end()) {
                history.push_back(Message(0, time(NULL), "user" + to_string(userId) + " joined to chat!"));
                firstReadMessage[userId] = lastReadMessage[userId] = history.size() - 1;
            }
            QUrl url(req.url().c_str());
            int l;
            if (url.queryItemValue("all") == "true")
                l = firstReadMessage[userId];
            else
                l = lastReadMessage[userId] + 1;

            HttpResponse r(200, "OK", req.version());
            if (req.isKeepAlive())
                r.addHeader("Connection", "Keep-Alive");

            r.addHeader("Set-Cookie", "user=" + to_string(userId) + "; expires=Fri, 31 Dec 2099 23:59:59 GMT;");
            r.addHeader("Set-Cookie", "hash=" + to_string(hash(userId)) + "; expires=Fri, 31 Dec 2099 23:59:59 GMT;");
            r.setBody(packageToJsonHistory(l, history.size() - 1));
            lastReadMessage[userId] = history.size() - 1;
            resp.response(r);
        } else {
            HttpResponse r(401, "Unauthorized", req.version());
            if (req.isKeepAlive())
                r.addHeader("Connection", "Keep-Alive");
            resp.response(r);
        }
    });
}

int ChatServer::start(int port) {
    return httpServer->start(port);
}

std::string ChatServer::getStringByFile(const char* name) {
    ifstream in(name);
    std::string s, ret;
    while (getline(in, s))
        ret += s + "\n";
    return ret;
}

ChatServer::UserType ChatServer::getUserIdByCookie(std::string cookie) {
    if (cookie == "")
        return ++numUsers;
    std::string userId;
    int i = cookie.find(COOKIE_USER);
    if (i == -1)
        return 0;
    i += 5;
    while (i < cookie.size() && cookie[i] != '&')
        userId += cookie[i++];
    if (userId.size() == 0)
        return 0;

    int j = cookie.find(COOKIE_HASH);
    if (j == -1)
        return 0;
    j += 5;
    std::string h;
    while (j < cookie.size() && cookie[j] != '&')
        h += cookie[j++];
    if (h.size() == 0)
        return 0;
    if (hash(atol(userId.c_str())) != atol(h.c_str()))
        return 0;
    return atol(userId.c_str());
}

std::string ChatServer::getMessage(const std::string& str) {
    return str.substr(8, str.size() - 8);
}

ChatServer::UserType ChatServer::hash(UserType userId) {
    static UserType shift = rand();
    UserType ret = 0;
    userId *= shift;
    UserType P = max((unsigned)2, (userId % 100) / 10), pw = 1;
    if (userId == 0) userId++;
    while (userId > 0) {
        ret += pw * (userId % 10);
        userId /= 10;
        pw *= P;
    }
    return ret;
}

std::string ChatServer::packageToJsonHistory(int l, int r) {
    std::string ret = "{\"messages\": [";
    for (; l <= r; ++l)
        if (l == r)
            ret += history[l].toJson();
        else
            ret += history[l].toJson() + ", ";
    return ret + "]}";
}

ChatServer::~ChatServer()
{
    delete httpServer;
}


std::string ChatServer::Message::toJson() {
    return "{\"from\": " + to_string(from) + ", \"timestamp\": " + to_string(time) + ", \"text\": \"" + text + "\"}";
}

