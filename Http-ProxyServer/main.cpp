#include <stdlib.h>
#include "application.h"
#include "ChatServer/chatserver.h"

using namespace std;

int main(int, char**) {
    Application app;
    srand(time(NULL));
    ChatServer *server = new ChatServer(&app);
    int status;
    if ((status = server->start(3334)) != 0) {
        cout << "Server couldn't start" << endl;
        return status;
    } else {
        cout << "Server started on port 3334" << endl;
        Application::setExitHandler([=]() {
            delete server;
        });
        return app.exec();
    }
}
