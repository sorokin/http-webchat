#include "application.h"
#include <ChatServer/chatserver.h>

#include <iostream>
#include <fstream>
using namespace std;

int main(int, char**) {
    Application app;
    ChatServer *server = new ChatServer(&app);
    cout << "Status = " << server->start(3334) << endl;
    Application::setAbortHandler([=]() {
        delete server;
    });
    return  app.exec();
}
