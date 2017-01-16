#include "http_server.h"

HttpServer::ResponseSocket::ResponseSocket(TcpServerSocket& socket): socket(socket) {
    valid = true;
}

void HttpServer::ResponseSocket::close() {
    socket.close();
    valid = false;
}

void HttpServer::ResponseSocket::end(HttpResponse& response) {
    if (!valid) {
        throw std::runtime_error("The response can't be sent twice");
    }

    response.finish();
    socket.write(response.to_string());
    valid = false;
}

HttpServer::RequestHandler HttpServer::defaultHandler = [](const HttpRequest& request, ResponseSocket responseSocket) {
    HttpResponse response(request.getMethod(),
                          (request.getVersion() == Http::VERSION1_0) ? Http::VERSION1_0 : Http::VERSION1_1,
                          405, "Method Not Allowed");
    responseSocket.end(response);
};

HttpServer::HttpServer(uint16_t port): listener(TcpAcceptSocket("127.0.0.1", port, [this](TcpServerSocket* socket) {
            HttpRequest* request = NULL;

            socket->setReceivedDataHandler([=](std::deque<char>& dataDeque) mutable {
                while (!dataDeque.empty() && socket->isOpened()) {
                    if (request == NULL) {
                        request = new HttpRequest();
                    }

                    if (request->getState() == HttpMessage::State::START
                        || request->getState() == HttpMessage::State::HEADER) {
                        std::deque<char>::iterator lf;
                        while ((request->getState() == HttpMessage::State::START
                                || request->getState() == HttpMessage::State::HEADER)
                               && (lf = std::find(dataDeque.begin(), dataDeque.end(), '\n')) != dataDeque.end()) {
                            request->append(std::string(dataDeque.begin(), lf - 1));
                            dataDeque.erase(dataDeque.begin(), lf + 1);
                        }
                    }

                    if (request->getState() == HttpMessage::State::BODY) {
                        size_t currentBodySize = request->getBodySize();
                        size_t declaredBodySize = request->getDeclaredBodySize();
                        size_t charsToGet = std::min(declaredBodySize - currentBodySize, dataDeque.size());

                        request->append(std::string(dataDeque.begin(), dataDeque.begin() + charsToGet));
                        dataDeque.erase(dataDeque.begin(), dataDeque.begin() + charsToGet);
                    }

                    if (request->getState() == HttpMessage::State::FINISHED) {
                        try {
                            processRequest(socket, *request);
                        } catch (const std::exception& exception) {
                            std::cerr << "Couldn't process a request: " << exception.what() << std::endl;
                            socket->close();
                        }
                        delete request;
                        request = NULL;
                    } else if (request->getState() == HttpMessage::State::INVALID) {
                        delete request;
                        request = NULL;
                    }
                }
            });
        })) {
    try {
        tfd = _m1_system_call(timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK), "Couldn't create a timer fd");

        struct itimerspec its = {};
        its.it_interval.tv_sec = 5;
        its.it_interval.tv_nsec = 0;
        its.it_value.tv_sec = 5;
        its.it_interval.tv_nsec = 0;
        _m1_system_call(timerfd_settime(tfd, 0, &its, NULL), "Couldn't run the timer fd");

        Poller::setHandler(tfd, [=](epoll_event event) {
            for (std::set<TcpServerSocket*>::iterator it = sockets.begin(); it != sockets.end(); ++it) {
                TcpServerSocket* socket = *it;
                if (!socket->isOpened()) {
                    delete socket;
                    sockets.erase(it++);
                }
            }

            uint64_t time;
            _m1_system_call(read(tfd, &time, sizeof time), "Couldn't read timer fd");
        }, EPOLLIN);
    } catch (const std::exception& exception) {
        listener.close();
        throw exception;
    }
}

HttpServer::~HttpServer() {
    for (std::set<TcpServerSocket*>::iterator it = sockets.begin(); it != sockets.end(); ++it) {
        delete *it;
    }

    try {
        Poller::removeHandler(tfd);
        _m1_system_call(::close(tfd), "Timer fd (fd " + std::to_string(tfd) + ") was closed incorrectly");
    } catch (const std::exception& exception) {
        std::cerr << "Exception while closing timer fd (fd " << tfd << "): " << exception.what() << std::endl;
    }
}

void HttpServer::processRequest(TcpServerSocket* socket, const HttpRequest& request) {
    bool ok = false;
    for (size_t i = 0; i < matchers.size(); ++i) {
        if (matchers[i].first.match(request)) {
            matchers[i].second(request, ResponseSocket(*socket));
            ok = true;
            break;
        }
    }

    if (!ok) {
        for (size_t i = 0; i < commonMatchers.size(); ++i) {
            if (commonMatchers[i].first.match(request)) {
                commonMatchers[i].second(request, ResponseSocket(*socket));
                ok = true;
                break;
            }
        }
    }

    if (!ok) {
        defaultHandler(request, ResponseSocket(*socket));
    }

    sockets.insert(socket);
}

void HttpServer::addRouteMatcher(const RouteMatcher& matcher, const HttpServer::RequestHandler& requestHandler) {
    if (matcher.getUri() == "*") {
        commonMatchers.push_back(std::make_pair(matcher, requestHandler));
    } else {
        matchers.push_back(std::make_pair(matcher, requestHandler));
    }
}
