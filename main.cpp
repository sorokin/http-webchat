#include "ChatServer/chat_server.h"

using namespace std;

const uint16_t PORT = 3334;

int main(int, char**) {
    try {
        Poller poller;
        ChatServer server(PORT);
        cout << "Server started on port " << PORT << endl;
        Poller::poll();
        return 0;
    } catch (std::exception& exception) {
        cerr << "Exception: " << exception.what() << endl;
        return 1;
    }
}
