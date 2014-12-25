#include "application.h"
#include <ChatServer/chatserver.h>

#include <iostream>
#include <fstream>
using namespace std;

int main(int, char**) {
    Application app;
    srand(time(NULL));
    ChatServer *server = new ChatServer(&app);
    cout << "Status = " << server->start(3333) << endl;
    Application::setAbortHandler([=]() {
        delete server;
    });
    //cerr << ChatServer::Message(1, 11111111, "hello").toJson() << endl;
    return  app.exec();
}
