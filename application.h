#ifndef APPLICATION_H
#define APPLICATION_H

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <vector>
#include <signal.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>

#include "common.h"

using namespace std;

class Application
{
public:
    typedef std::function <void(epoll_event)> Handler;
    typedef std::function <void()> ExitHandler;

    Application();

    int setHandler(int fd, Handler handler, uint32_t flags);
    void removeHandler(int fd);
    void changeFlags(int fd, uint32_t flags);
    int exec();
    ~Application();

    static void sigHandler(int) {
        ::eventfd_write(stopFD, 1);
    }

//    static void setExitHandler(ExitHandler handler) {
//        exitHandler = handler;
//        signal(SIGINT, Application::sigHandler);
//        signal(SIGTERM, Application::sigHandler);
//    }
private:
//    static ExitHandler exitHandler;

    const size_t MAX_EVENTS;
    int mainLoopFD;
    static int stopFD;
    std::vector <epoll_event> events;
    std::map <int, Handler> handlers;
};

#endif // APPLICATION_H
