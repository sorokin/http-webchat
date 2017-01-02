#include "tcp_server_socket.h"

const size_t TcpServerSocket::READ_BUFFER_SIZE = 4096;
const size_t TcpServerSocket::WRITE_BUFFER_SIZE = 4096;

TcpServerSocket::TcpServerSocket(Poller& poller, int fd, const std::string& host, uint16_t port):
        TcpSocket(poller, fd, host, port) {
    try {
        poller.setHandler(fd, [this](const epoll_event& event) {
            eventHandler(event);
        }, EPOLLIN);
    } catch (std::string error) {
        ::close(fd);
        throw error;
    } catch (std::bad_alloc& ba) {
        ::close(fd);
        throw ba;
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
        } catch (std::string error) {
            std::cerr << "Error while reading from socket (fd " << fd << "), closing socket: "
                      << error << std::endl;
            close();
        } catch (std::bad_alloc& ba) {
            std::cerr << "Allocation exception while reading from socket (fd " << fd << "), closing socket: "
                      << ba.what() << std::endl;
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
                    poller.setEvents(fd, EPOLLIN);
                }
            }
        } catch (std::string error) {
            std::cerr << "Error while writing into socket (fd " << fd << "), closing socket: "
                      << error << std::endl;
            close();
        } catch (std::bad_alloc& ba) {
            std::cerr << "Allocation exception while writing into socket (fd " << fd << "), closing socket: "
                      << ba.what() << std::endl;
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
        } catch (std::string error) {
            std::cerr << "Error while processing received data from socket (fd " << fd << "), closing socket: "
                      << error << std::endl;
            close();
        } catch (std::bad_alloc& ba) {
            std::cerr << "Allocation exception while processing received data from socket (fd " << fd
                      << "), closing socket: " << ba.what() << std::endl;
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
            poller.setEvents(fd, EPOLLIN | EPOLLOUT);
        } catch (std::string error) {
            close();
            throw "Error while writing to buffer of socket (fd " + std::to_string(fd) + "), closing socket: " + error;
        } catch (std::exception& exception) {
            close();
            throw "Error while writing to buffer of socket (fd " + std::to_string(fd) + "), closing socket: "
                  + exception.what();
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
