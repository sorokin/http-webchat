#include "application.h"

using namespace std;

int Application::stopFD = 0;

//Application::ExitHandler Application::exitHandler;

Application::Application():MAX_EVENTS(128), events(MAX_EVENTS) {
    struct sigaction sa = {};
    sa.sa_handler = Application::sigHandler;
    sigset_t ss;
    string seMsg = "Couldn't set signal handler";
    _m1_system_call(sigemptyset(&ss), seMsg);
    _m1_system_call(sigaddset(&ss, SIGINT), seMsg);
    _m1_system_call(sigaddset(&ss, SIGTERM), seMsg);
    sa.sa_mask = ss;
    _m1_system_call(sigaction(SIGINT, &sa, 0), seMsg);
    _m1_system_call(sigaction(SIGTERM, &sa, 0), seMsg);

//    mainLoopFD = epoll_create1(0);
    mainLoopFD = _m1_system_call(epoll_create1(0), "Couldn't run epoll fd");
//    stopFD = eventfd(0, EFD_NONBLOCK);
    stopFD = _m1_system_call(eventfd(0, EFD_NONBLOCK), "Couldn't run terminating fd");

    epoll_event ev = {};
    ev.data.fd = stopFD;
    ev.events = EPOLLIN;
//    epoll_ctl(mainLoopFD, EPOLL_CTL_ADD, stopFD, &ev);
    _m1_system_call(epoll_ctl(mainLoopFD, EPOLL_CTL_ADD, stopFD, &ev), "Couldn't add terminating fd to polling");
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
    for (;;) {
        int n = epoll_wait(mainLoopFD, events.data(), (int) events.size(), -1);
        for (int i = 0; i < n; ++i)
            if (events[i].data.fd != stopFD) {
                handlers[events[i].data.fd](events[i]);
            } else {
//                if (exitHandler) {
//                    exitHandler();
//                }
                cout << "Server is closing" << endl;
                return 0;
            }
    }
}

Application::~Application() {
    try {
        _m1_system_call(epoll_ctl(mainLoopFD, EPOLL_CTL_DEL, stopFD, NULL),
                        "Couldn't remove terminating fd from polling");
        _m1_system_call(close(stopFD), "Couldn't close terminating fd");
        _m1_system_call(close(mainLoopFD), "Couldn't close polling fd");
    } catch (string error) {
        cout << "Something wrong happened while destructing the application" << error << endl;
    }
//    int s = ::close(mainLoopFD);
//    assert(s == 0);
}
