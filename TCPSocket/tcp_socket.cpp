#include "tcp_socket.h"

const int TcpSocket::NONE = -1;

TcpSocket::TcpSocket(Poller& poller, int fd, const std::string& host, uint16_t port):
        poller(poller), fd(fd), host(host), port(port) {
    try {
        std::string error = "Couldn't make socket (fd " + std::to_string(fd) + ") non-blocking";
        int flags = _m1_system_call(fcntl(fd, F_GETFL, 0), error);
        _m1_system_call(fcntl(fd, F_SETFL, flags | O_NONBLOCK), error);
    } catch (std::string error) {
        ::close(fd);
        throw error;
    }
}

TcpSocket::~TcpSocket() {
    close();
}

void TcpSocket::close() {
    if (fd != NONE) {
        poller.removeHandler(fd);
        try {
            _m1_system_call(::close(fd), "Socket (fd " + std::to_string(fd) + ") was closed incorrectly");
        } catch (std::string error) {
            std::cerr << error << std::endl;
        }
        fd = NONE;
    }
}