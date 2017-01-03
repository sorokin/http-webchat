#include "tcp_server_socket.h"

const size_t TcpServerSocket::READ_BUFFER_SIZE = 4096;
const size_t TcpServerSocket::WRITE_BUFFER_SIZE = 4096;

TcpServerSocket::TcpServerSocket(int fd, const std::string& host, uint16_t port): TcpSocket(fd, host, port) {
    try {
        Poller::setHandler(fd, [this](const epoll_event& event) {
            eventHandler(event);
        }, EPOLLIN);
    } catch (std::exception& exception) {
        ::close(fd);
        throw exception;
    }
}

void TcpServerSocket::eventHandler(const epoll_event& event) {
    if (event.events & EPOLLHUP) {
        close();
        return;
    }

    if (event.events & EPOLLIN) {
        try {
            char buf[READ_BUFFER_SIZE];

            ssize_t readCount = ::read(fd, buf, READ_BUFFER_SIZE);
            if (readCount == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
                close();
                return;
            } else if (readCount > 0) {
                inBuffer.insert(inBuffer.end(), buf, buf + readCount);
                if (receivedDataHandler) {
                    receivedDataHandler(inBuffer);
                }
            }
        } catch (std::exception& exception) {
            std::cerr << "Exception while reading from socket (fd " << fd << "), closing socket: "
                      << exception.what() << std::endl;
            close();
            return;
        }
    }

    if (event.events & EPOLLOUT) {
        try {
            char buf[WRITE_BUFFER_SIZE];
            size_t amount = std::min(outBuffer.size(), WRITE_BUFFER_SIZE);
            std::copy(outBuffer.begin(), outBuffer.begin() + amount, buf);

            ssize_t writtenCount = ::write(fd, buf, amount);
            if (writtenCount == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
                close();
                return;
            } else if (writtenCount > 0) {
                outBuffer.erase(outBuffer.begin(), outBuffer.begin() + writtenCount);
                if (outBuffer.empty()) {
                    Poller::setEvents(fd, EPOLLIN);
                }
            }
        } catch (std::exception& exception) {
            std::cerr << "Exception while writing into socket (fd " << fd << "), closing socket: "
                      << exception.what() << std::endl;
            close();
            return;
        }
    }
}

void TcpServerSocket::setReceivedDataHandler(SocketReceivedDataHandler socketReceivedDataHandler) {
    receivedDataHandler = socketReceivedDataHandler;
    if (socketReceivedDataHandler && !inBuffer.empty()) {
        try {
            receivedDataHandler(inBuffer);
        } catch (std::exception& exception) {
            std::cerr << "Exception while processing received data from socket (fd " << fd
                      << "), closing socket: " << exception.what() << std::endl;
            close();
        }
    }
}

void TcpServerSocket::setClosedHandler(SocketClosedHandler socketClosedHandler) {
    closedHandler = socketClosedHandler;
}

void TcpServerSocket::write(const std::string& data) {
    bool wasEmpty = outBuffer.empty();
    outBuffer.insert(outBuffer.end(), data.begin(), data.end());
    if (wasEmpty) {
        try {
            Poller::setEvents(fd, EPOLLIN | EPOLLOUT);
        } catch (std::exception& exception) {
            close();
            throw std::runtime_error("Exception while writing to buffer of socket (fd " + std::to_string(fd) + "), closing socket: "
                  + exception.what());
        }
    }
}

void TcpServerSocket::close() {
    if (closedHandler) {
        try {
            closedHandler();
        } catch (...) {}
        closedHandler = NULL;
    }
    TcpSocket::close();
}
