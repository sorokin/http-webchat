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
class SimpleServer: public HttpServer {
public:
    SimpleServer(Application *app):HttpServer(app){}
    void initializate() {
        cerr << "lol init" << endl;
    }
};

int main(int argc, char *argv[]) {
    Application app;
    freopen("output.txt", "w", stdout);
    SimpleServer *serv = new SimpleServer(&app);
    cerr << "Status = " << serv->start(3333) << endl;

    //HttpClient *cl = new HttpClient(&app);
    //HttpRequest *q = new HttpRequest(HttpRequest::POST, "www.google.ru");

    /*HttpClient *client = new HttpClient(&app);
    client->request(HttpRequest(HttpRequest::GET, "http://codeforces.ru"),
        [&](HttpResponse r) {
            string s = r.body();
            string inl;
            stringstream in(s);
            while (getline(in, inl))
                cout << inl << endl;
    });*/

    // [](const HttpResponse&) {});
    return app.exec();
}
