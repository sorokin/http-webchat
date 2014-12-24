#include "application.h"
#include <cassert>

#include <iostream>
using namespace std;

Application::AbortHandler Application::abortHandler;

Application::Application():MAX_EVENTS(128), events(MAX_EVENTS) {
    mainLoopFD = epoll_create1(0);
}

int Application::setHandler(int fd, Handler handler, __uint32_t flags) {
    epoll_event ev;
    memset(&ev, 0, sizeof ev);
    ev.data.fd = fd;
    ev.events = flags;
    int s;
    if (handlers.find(fd) != handlers.end()) {
        s = epoll_ctl(mainLoopFD, EPOLL_CTL_MOD, fd, &ev);
        handlers[fd] = handler;
    } else if ((s = epoll_ctl(mainLoopFD, EPOLL_CTL_ADD, fd, &ev)) == 0)
        handlers[fd] = handler;
    else
        printf("errror: setHandler:epoll_ctl\n");
    return s;
}

void Application::changeFlags(int fd, __uint32_t flags) {
    epoll_event event;
    memset(&event, 0, sizeof event);
    event.events = flags;
    event.data.fd = fd;
    epoll_ctl(mainLoopFD, EPOLL_CTL_MOD, fd, &event);
}

void Application::removeHandler(int fd) {
    handlers.erase(fd);
    epoll_ctl(mainLoopFD, EPOLL_CTL_DEL, fd, NULL);
}

int Application::exec() {
    for (;;) {
        int n = epoll_wait(mainLoopFD, events.data(), events.size(), -1);
        for (int i = 0; i < n; ++i)
            handlers[events[i].data.fd](events[i]);
    }
}

Application::~Application() {
    int s = ::close(mainLoopFD);
    assert(s == 0);
}
