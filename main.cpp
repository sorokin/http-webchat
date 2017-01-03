#include "ChatServer/chat_server.h"

using namespace std;

const uint16_t PORT = 3334;

int main(int, char**) {
    try {
        Poller::start();
        ChatServer server(PORT);
        cout << "Server started on port " << PORT << endl;
        Poller::poll();
        Poller::stop();
        return 0;
    } catch (std::exception& exception) {
        cerr << "Exception: " << exception.what() << endl;
        Poller::stop();
        return 1;
    }
}
