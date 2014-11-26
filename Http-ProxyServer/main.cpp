#include <application.h>
#include <TCP-socket/tcpserversocket.h>
#include <iostream>
#include <QDebug>
using namespace std;

bool IS_SERV = 0;
int main(int argc, char *argv[]) {
if (IS_SERV) {
    TcpServerSocket *serv = new TcpServerSocket();
    cout << "succ = " << serv->listen("127.0.0.1", 10003,
        [=]{
            TcpSocket* socket = serv->getPendingConnecntion();
            //socket->write("hello, client!");
            socket->setDataReceivedHandler([=] {
                char *str = socket->readString();
                printf("message: %s\n", str);
                socket->write(string(str) + string(" request OK"));
            });
       }) << endl;
}

if (!IS_SERV) {
    for (int i = 0; i < 100; ++i) {
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
    }
}
    return Application::instance()->exec();
}
