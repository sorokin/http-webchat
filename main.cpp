#include <stdlib.h>
#include "application.h"
#include "ChatServer/chatserver.h"

using namespace std;

//ChatServer* server;

int main(int, char**) {
    try {
        Application app;
        srand(time(NULL));
//        server = new ChatServer(&app);
        ChatServer server(&app);

//        int status;
//        if ((status = server->start(3334)) != 0) {
//        if ((status = server.start(3334)) != 0) {
//            cout << "Server couldn't start" << endl;
//            return status;
//        } else {
        server.start(3334);
        cout << "Server started on port 3334" << endl;
//            Application::setExitHandler([=]() {
//                delete server;
//            });
        return app.exec();
//        }
    } catch (bad_alloc ex) {
        cerr << "Couldn't allocate memory: " << ex.what() << endl;
        return 2;
    } catch (string err) {
        cerr << "Error happened: " << err << endl;
//        if (server != NULL) {
//            delete server;
//        }
        return 1;
    }
}
