#include <application.h>
#include <TCP-socket/tcpserversocket.h>
#include <iostream>
#include <QDebug>
#include <Http/httpclient.h>
#include <QUrl>
#include <QFile>
#include <sstream>
#include <Http/httpserver.h>

using namespace std;

int main(int argc, char *argv[]) {
    Application app;
    HttpServer *serv = new HttpServer(&app);
    serv->setMethodHandler("GET", [](HttpRequest req, HttpServer::Response resp) {
        HttpResponse r(200, "OK", req.version(), "<HTML><BODY>Сачок-пиздючок</BODY></HTML>");
        cerr << req.header("Connection") << endl;
        resp.response(r);
    });
    cerr << "Status = " << serv->start(3335) << " lol " << endl;
    Application::setAbortHandler([=]() {
        delete serv;
    });
    return  app.exec();
}
