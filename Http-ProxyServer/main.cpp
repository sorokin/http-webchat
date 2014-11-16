#include <application.h>
#include <TCP-socket/tcpserversocket.h>
#include <iostream>
#include <QDebug>

using namespace std;

int main(int argc, char *argv[]) {
    /*TcpServerSocket *serv = new TcpServerSocket();
    cout << "succ = " << serv->listen("127.0.0.1", 10002,
        [=]{
            TcpSocket* socket = serv->getPendingConnecntion();
            socket->setDataReceivedHandler([=] {
            char *str = socket->readString();
            printf("%s\n", str);
            });
       }) << endl;*/
    TcpSocket *socket = new TcpSocket();
    socket->setDataReceivedHandler([] {
                                   cout << "in data" << endl;
    });

    socket->setCloseConnectionHandler([] {cout << "disconnected" << endl;});

    cout << socket->connectToHost("127.0.0.1", 10001) << endl;
    cout << socket->write("hello") << endl;
    return Application::instance()->exec();
}
