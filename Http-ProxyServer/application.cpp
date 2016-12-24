#include "application.h"

using namespace std;

int Application::running = 0;
int Application::stopFD = 0;

Application::ExitHandler Application::exitHandler;

Application::Application():MAX_EVENTS(128), events(MAX_EVENTS) {
    mainLoopFD = epoll_create1(0);
    stopFD = eventfd(0, EFD_NONBLOCK);

    epoll_event ev = {};
    ev.data.fd = stopFD;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLHUP;
    epoll_ctl(mainLoopFD, EPOLL_CTL_ADD, stopFD, &ev);
}

int Application::setHandler(int fd, Handler handler, uint32_t flags) {
    epoll_event ev = {};
    //memset(&ev, 0, sizeof ev);
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

void Application::changeFlags(int fd, uint32_t flags) {
    epoll_event event = {};
    //memset(&event, 0, sizeof event);
    event.events = flags;
    event.data.fd = fd;
    epoll_ctl(mainLoopFD, EPOLL_CTL_MOD, fd, &event);
}

void Application::removeHandler(int fd) {
    handlers.erase(fd);
    epoll_ctl(mainLoopFD, EPOLL_CTL_DEL, fd, NULL);
}

int Application::exec() {
    for (; running == 0;) {
        int n = epoll_wait(mainLoopFD, events.data(), events.size(), -1);
        for (int i = 0; i < n; ++i)
            if (events[i].data.fd != stopFD)
                handlers[events[i].data.fd](events[i]);
    }
    if (exitHandler) {
        exitHandler();
    }
    return 0;
}

Application::~Application() {
    int s = ::close(mainLoopFD);
    assert(s == 0);
}
