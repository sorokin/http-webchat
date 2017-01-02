#include "http_server.h"

HttpServer::ResponseSocket::ResponseSocket(TcpServerSocket& socket): socket(socket) {
    valid = true;
}

void HttpServer::ResponseSocket::end(HttpResponse& response) {
    if (!valid) {
        throw "The response can't be sent twice";
    }

    socket.write(response.finish());
    valid = false;
}

HttpServer::RequestHandler HttpServer::defaultHandler = [](const HttpRequest& request, ResponseSocket responseSocket) {
    HttpResponse response(request.getMethod(),
                          (request.getVersion() == Http::version1_0) ? Http::version1_0 : Http::version1_1,
                          405, "Method Not Allowed");
    responseSocket.end(response);
};

HttpServer::HttpServer(Poller& poller, uint16_t port): poller(poller),
        listener(TcpAcceptSocket(poller, "127.0.0.1", port, [this](TcpServerSocket* socket) {
            HttpRequest* request = NULL;

            socket->setReceivedDataHandler([socket, &request, this](std::deque<char>& dataDeque) {
                while (!dataDeque.empty() && socket->isOpened()) {
                    if (request == NULL) {
                        request = new HttpRequest();
                    }

                    if (request->getState() == HttpMessage::State::FINISHED
                        || request->getState() == HttpMessage::State::INVALID) {
                        delete request;
                        request = NULL;
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
                        } catch (std::string error) {
                            std::cerr << "Couldn't process a request: " << error << std::endl;
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

        poller.setHandler(tfd, [=](epoll_event event) {
            std::set<TcpServerSocket*>::iterator it_prev = sockets.begin();
            for (std::set<TcpServerSocket*>::iterator it = it_prev; it != sockets.end(); it_prev = it) {
                ++it;
                TcpServerSocket* socket = *it_prev;
                if (!socket->isOpened()) {
                    delete socket;
                    sockets.erase(it_prev);
                }
            }
        }, EPOLLIN);
    } catch (std::string error) {
        listener.close();
        throw error;
    } catch (std::exception exception) {
        listener.close();
        throw exception;
    }
}

HttpServer::~HttpServer() {
    for (std::set<TcpServerSocket*>::iterator it = sockets.begin(); it != sockets.end(); ++it) {
        delete *it;
    }

    try {
        poller.removeHandler(tfd);
        _m1_system_call(::close(tfd), "Timer fd (fd " + std::to_string(tfd) + ") was closed incorrectly");
    } catch (std::string error) {
        std::cerr << "Error while closing timer fd (fd " << tfd << "): " << error << std::endl;
    } catch (std::exception& exception) {
        std::cerr << "Bad allocation while closing timer fd (fd " << tfd << "): " << exception.what() << std::endl;
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

    if (!request.shouldKeepAlive()) {
        socket->close();
    } else {
        sockets.insert(socket);
    }
}

void HttpServer::addRouteMatcher(const RouteMatcher& matcher, const HttpServer::RequestHandler& requestHandler) {
    if (matcher.getUri() == "*") {
        commonMatchers.push_back(std::make_pair(matcher, requestHandler));
    } else {
        matchers.push_back(std::make_pair(matcher, requestHandler));
    }
}
