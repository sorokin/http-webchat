#ifndef APPLICATION_H
#define APPLICATION_H
#include <sys/epoll.h>
#include <functional>
#include <map>
#include <vector>
#include <unistd.h>
#include <cstring>

class Application
{
public:
    typedef std::function <void(epoll_event)> Handler;
    Application();

    int setHandler(int fd, Handler handler, __uint32_t flags);
    void removeHandler(int fd);
    void changeFlags(int fd, __uint32_t flags);
    int exec();
    ~Application();
private:
    const int MAX_EVENTS;
    int mainLoopFD;
    std::vector <epoll_event> events;
    std::map <int, Handler> handlers;
};

#endif // APPLICATION_H
