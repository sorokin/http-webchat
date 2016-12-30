#include "chatserver.h"

ChatServer::ChatServer(Application* app):
    httpServer(new HttpServer(app))
{
    httpServer->addRouteMatcher(RouteMatcher(), [](HttpRequest req, HttpServer::Response res) {
        std::string filename = req.path();
        if (filename == "/")
            filename = "/index.html";
        filename = "." + filename;
        ifstream in(filename.c_str());
        if (!in) {
            logError(req, 404, "Not Found");
            HttpResponse r(404, "Not Found", req.version(),
                           "<html><head>"
                           "<title>404 Not Found</title>"
                           "</head><body>"
                           "<h1>Not Found</h1>"
                           "<p>The requested URL " + req.path() + " was not found on this server.</p>"
                           "<hr></body></html>");
            res.response(r);
            return;
        }

        std::string s, ret;
        while (getline(in, s))
            ret += s + "\n";

        size_t pos = filename.find(".");
        std::string type = filename.substr(pos + 1, filename.size() - pos - 1);

        HttpResponse r(200, "OK", req.version(), ret);
        if (req.notKeptAlive())
            r.addHeader("Connection", "Keep-Alive");
        if (type == "js") {
            r.addHeader("Content-Type", "application/javascript");
        } else if (type == "html") {
            r.addHeader("Content-Type", "text/html");
        }
        res.response(r);
    });

    httpServer->addRouteMatcher(RouteMatcher("POST", "/login"), [this](HttpRequest req, HttpServer::Response res) {
        map<string, string> form = req.parseForm(req.body());
        if (form.size() != 1 || form.find("username") == form.end()) {
            logError(req, 400, "Bad Request (invalid body or no username)");
            HttpResponse r(400, "Bad Request", req.version());
            res.response(r);
            return;
        }

        string username = form["username"];
        if (username == "") {
            logError(req, 400, "Bad Request (empty username)");
            HttpResponse r(400, "Bad Request", req.version());
            res.response(r);
            return;
        }

        if (firstMessage.find(username) == firstMessage.end()) {
            size_t greetingNumber = history.size();
            history.push_back(Message(username, time(NULL), "User " + username + " joined to chat!"));
            firstMessage[username] = greetingNumber;
        }

        HttpResponse r(200, "OK", req.version());
        if (req.notKeptAlive())
            r.addHeader("Connection", "Keep-Alive");
        res.response(r);
    });

    httpServer->addRouteMatcher(RouteMatcher("GET", "/messages"), [this](HttpRequest req, HttpServer::Response res) {
        QUrlQuery query(QUrl(req.url().c_str()));
        string username = query.queryItemValue("username").toUtf8().constData();
        string allMessages = query.queryItemValue("all").toUtf8().constData();

        if (username == "" || (allMessages != "" && allMessages != "true" && allMessages != "false")) {
            logError(req, 400, "Bad Request (empty/invalid username or invalid all messages indicator)");
            HttpResponse r(400, "Bad Request", req.version());
            res.response(r);
            return;
        }

        bool isAll = (allMessages == "true");
        size_t begin;
        if (isAll || firstUnreadMessage.find(username) == firstUnreadMessage.end()) {
            begin = firstMessage[username];
        } else {
            begin = firstUnreadMessage[username];
        }
        firstUnreadMessage[username] = history.size();

        HttpResponse r(200, "OK", req.version());
        if (req.notKeptAlive())
            r.addHeader("Connection", "Keep-Alive");
        r.addHeader("Content-Type", "application/json; charset=utf-8");
        r.setBody(packageToJsonHistory(begin, history.size()));
        res.response(r);
    });

    httpServer->addRouteMatcher(RouteMatcher("POST", "/messages"), [this](HttpRequest req, HttpServer::Response res) {
        QUrlQuery url(QUrl(req.url().c_str()));
        string username = url.queryItemValue("username").toUtf8().constData();
        if (username == "") {
            logError(req, 400, "Bad Request (empty username)");
            HttpResponse r(400, "Bad Request", req.version());
            res.response(r);
            return;
        }

        string message = getMessage(req.body());
        if (message == "") {
            logError(req, 400, "Bad Request (empty or invalid message)");
            HttpResponse r(400, "Bad Request", req.version());
            res.response(r);
            return;
        }

        cout << "User \"" << username << "\" sent message \"" << message << "\"" << endl;
        history.push_back(Message(username, time(NULL), message));

        HttpResponse r(200, "OK", req.version());
        if (req.notKeptAlive())
            r.addHeader("Connection", "Keep-Alive");
        res.response(r);
    });

    httpServer->addRouteMatcher(RouteMatcher("OPTIONS", "*"), [](HttpRequest req, HttpServer::Response res) {
        HttpResponse r(200, "OK", req.version());
        if (req.notKeptAlive())
            r.addHeader("Connection", "Keep-Alive");
        r.addHeader("Access-Control-Allow-Headers", "Content-Type");
        r.addHeader("Access-Control-Allow-Methods", "GET, OPTIONS, POST");
        res.response(r);
    });
}

int ChatServer::start(uint16_t port) {
    return httpServer->start(port);
}

std::string ChatServer::getMessage(const std::string& str) {
    if (str.substr(0, 8) != "message=") {
        return "";
    } else {
        return str.substr(8, str.size() - 8);
    }
}

std::string ChatServer::Message::toJson() {
    return "{\"from\": \"" + from + "\", \"timestamp\": " + to_string(time) + ", \"text\": \"" + text + "\"}";
}

std::string ChatServer::packageToJsonHistory(size_t l, size_t r) {
    std::string ret = "{\"messages\": [";
    for (; l < r; ++l)
        if (l == r - 1) {
            ret += history[l].toJson();
        } else {
            ret += history[l].toJson() + ", ";
        }
    return ret + "]}";
}

void ChatServer::logError(const HttpRequest& req, int code, const string& res) {
    cout << req.method() << " request to \"" << req.path() << "\"" << endl;
    cout << "  Body: \"" << req.body() << "\"" << endl;
    cout << "  Result: " << res << ", sending code " << code << endl;
}

ChatServer::~ChatServer()
{
    delete httpServer;
}
