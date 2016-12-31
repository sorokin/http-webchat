#include <iostream>
#include "poller.h"

Poller::Poller() {
    struct sigaction sa = {};
    sa.sa_handler = (__sighandler_t) &Poller::signalHandler;
    sigset_t ss;
    std::string seMsg = "Couldn't set signal handler";
    _m1_system_call(sigemptyset(&ss), seMsg);
    _m1_system_call(sigaddset(&ss, SIGINT), seMsg);
    _m1_system_call(sigaddset(&ss, SIGTERM), seMsg);
    sa.sa_mask = ss;
    _m1_system_call(sigaction(SIGINT, &sa, 0), seMsg);
    _m1_system_call(sigaction(SIGTERM, &sa, 0), seMsg);

    efd = _m1_system_call(epoll_create1(0), "Couldn't run the polling fd");
    try {
        sfd = _m1_system_call(eventfd(0, EFD_NONBLOCK), "Couldn't run the stopping fd");
        try {
            epoll_event ev = {};
            ev.data.fd = sfd;
            ev.events = EPOLLIN;
            _m1_system_call(epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &ev), "Couldn't add the stopping fd to polling");
        } catch (std::string error) {
            close(sfd);
            throw error;
        }
    } catch (std::string error) {
        close(efd);
        throw error;
    }
}

void Poller::setHandler(int fd, Poller::Handler handler, uint32_t events) {
    epoll_event ev = {};
    ev.data.fd = fd;
    ev.events = events;

    if (handlers.find(fd) == handlers.end()) {
        _m1_system_call(epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev),
                        "Couldn't add fd " + std::to_string(fd) + " to polling");
        try {
            handlers[fd] = handler;
        } catch (...) {
            handlers.erase(fd);
            _m1_system_call(epoll_ctl(efd, EPOLL_CTL_DEL, fd, &ev),
                            "Couldn't remove fd " + std::to_string(fd) + " from polling");
        }
    } else {
        throw "This fd is already handled by a handler";
    }
}

void Poller::setEvents(int fd, uint32_t events) {
    epoll_event ev = {};
    ev.data.fd = fd;
    ev.events = events;
    _m1_system_call(epoll_ctl(efd, EPOLL_CTL_MOD, fd, &ev),
                    "Couldn't change polling event set of fd " + std::to_string(fd));
}

void Poller::removeHandler(int fd) {
    if (handlers.erase(fd) == 1) {
        _m1_system_call(epoll_ctl(efd, EPOLL_CTL_DEL, fd, NULL),
                        "Couldn't remove fd " + std::to_string(fd) + " from polling");
    } else {
        throw "There was nothing to remove";
    }
}

void Poller::poll() {
    while (true) {
        int n = epoll_wait(efd, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; ++i) {
            if (events[i].data.fd != sfd) {
                handlers[events[i].data.fd](events[i]);
            } else {
                return;
            }
        }
    }
}

Poller::~Poller() {
    int res = epoll_ctl(efd, EPOLL_CTL_DEL, sfd, NULL);
    if (res == -1) {
        std::cerr << "Couldn't remove the stopping fd from polling: " << strerror(errno) << std::endl;
    }

    res = close(sfd);
    if (res == -1) {
        std::cerr << "Couldn't close the stopping fd: " << strerror(errno) << std::endl;
    }

    res = close(efd);
    if (res == -1) {
        std::cerr << "Couldn't close the polling fd: " << strerror(errno) << std::endl;
    }
}

void Poller::signalHandler(int) {
    eventfd_write(sfd, 1);
}
