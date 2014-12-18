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
    freopen("output.txt", "w", stdout);
    HttpServer *serv = new HttpServer(&app);
    serv->setMethodHandler("GET", [](HttpRequest req, HttpServer::Response resp) {
        HttpResponse r(200, "OK", req.version(), "<HTML><BODY>Сачок-пиздючок</BODY></HTML>");
        resp.response(r);
    });
    cerr << "Status = " << serv->start(3333) << endl;
    return app.exec();
}
