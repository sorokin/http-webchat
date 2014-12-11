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
    //SimpleServer *serv = new SimpleServer(&app);
    //cerr << "Status = " << serv->start(3333) << endl;

    //HttpClient *cl = new HttpClient(&app);
    //HttpRequest *q = new HttpRequest(HttpRequest::POST, "www.google.ru");

    /*
    bool IS_SERV = 0;
    if (IS_SERV) {
        TcpServerSocket *serv = new TcpServerSocket();
        TcpServerSocket::ConnectedState st = serv->listen("127.0.0.1", 10003,
            [=]{
                TcpSocket* socket = serv->getPendingConnecntion();
                socket->setDataReceivedHandler([=] {
                    string str = socket->readString();
                    printf("message: %s\n", str.c_str());
                    socket->write(string(str) + string(" request OK"));
                });
           });
        printf("state = %d\n", st);
    }

    if (!IS_SERV) {
        for (int i = 0; i < 10; ++i) {
            TcpSocket *sock = new TcpSocket();
            char ss[20];
            sprintf(ss, "%d", i);
            sock->connectToHost("127.0.0.1", 10003);
            sock->setDataReceivedHandler([=] {
                    cout << "onReceived\n";
                    cout << string("in data ") + string(sock->readString()) << endl;
            });
            sock->setCloseConnectionHandler([] {
                                            printf("disconnected\n");});
                char s[20];
                sprintf(s, "lolka %d", i + 1);
                sock->write(s, strlen(s));
                //sock->close();
            }
    }*/

    HttpClient *client = new HttpClient(&app);
    client->request(HttpRequest(HttpRequest::GET, "http://codeforces.ru"),
        [&](HttpResponse r) {
            string s = r.body();
            string inl;
            stringstream in(s);
            while (getline(in, inl))
                cout << inl << endl;
    });
    // [](const HttpResponse&) {});
    return app.exec();
}
