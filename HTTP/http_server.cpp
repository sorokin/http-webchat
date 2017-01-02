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

HttpServer::HttpServer(Poller& poller, uint16_t port):
        listener(TcpAcceptSocket(poller, "127.0.0.1", port, [this](TcpSocket* socket) {
            TcpServerSocket* serverSocket = (TcpServerSocket*) socket;
            HttpRequest* request = NULL;

            serverSocket->setReceivedDataHandler([serverSocket, &request, this](std::deque<char>& dataDeque) {
                while (!dataDeque.empty()) {
                    if (request == NULL) {
                        request = new HttpRequest();
                    }

                    if (request->getState() == HttpMessage::State::FINISHED
                        || request->getState() == HttpMessage::State::INVALID) {
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
                        processRequest(serverSocket, *request);
                        delete request;
                        request = NULL;
                    } else if (request->getState() == HttpMessage::State::INVALID) {
                        delete request;
                        request = NULL;
                    }
                }
            });

            serverSocket->setClosedHandler([serverSocket, this]() {
                sockets.erase(serverSocket);
                delete serverSocket;
            });
        })) {}

HttpServer::~HttpServer() {
    for (std::set<TcpServerSocket*>::iterator it = sockets.begin(); it != sockets.end(); ++it) {
        delete *it;
    }
}

void HttpServer::processRequest(TcpServerSocket* socket, const HttpRequest& request) {
    if (request.getState() != HttpMessage::State::FINISHED) {
        throw "The request is invalid or wasn't finished";
    }

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
            commonMatchers[i].second(request, ResponseSocket(*socket));
            ok = true;
            break;
        }
    }

    if (!ok) {
        defaultHandler(request, ResponseSocket(*socket));
    }

    if (!request.shouldKeepAlive()) {
        delete socket;
    } else {
        sockets.insert(socket);
    }
}

void HttpServer::addRouteMatcher(const RouteMatcher& matcher, const HttpServer::RequestHandler& requestHandler) {
    matchers.push_back(std::make_pair(matcher, requestHandler));
}
