#include "config.h"
#include "server/server.hpp"
#include "request/request.hpp"
#include "pool/pool.hpp"
#include "response/response.hpp"

#include <unordered_map>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/epoll.h>
#include <thread>
#include <unistd.h>
#include <functional>
#include <trie.hpp>

extern "C" {
#include <crashtrace.h>
#include <logger.h>
}

typedef struct {
    Request* request;
    int fd;
    int epollFd;
    Trie<std::function<void(void*)>>* router;
    int eventFlags;
    struct epoll_event* event;
    Server *server;
} TransportData;

typedef struct {
    Request* request;
    Response* response;
} RouteData;

void handleRequest(void* data)
{
    TransportData* transportData = (TransportData*)data;
    Request* request = transportData->request;
    int fd = transportData->fd;
    int epollFd = transportData->epollFd;
    Trie<std::function<void(void*)>>* router = transportData->router;
    int eventFlags = transportData->eventFlags;
    struct epoll_event* event = transportData->event;
    Server *server = transportData->server;
    auto func = router->get(request->route);
    Response response;
    RouteData routeData;
    routeData.request = request;
    routeData.response = &response;
    if (func) {
        func(&routeData);
    }
    response.sendResponse(fd);
    close(fd);
    server->decreaseEpollCount();
    
    delete transportData;
}

int main(int argc, char** argv)
{
    crashtrace_init(argv[0]);
    int epollFd = epoll_create(MAX_EPOLL_EVENTS);

    if (epollFd == -1) {
        logError("Error creating epoll file descriptor");
        return 1;
    }

    struct epoll_event events[MAX_EPOLL_EVENTS];

    Server server(epollFd, SERVER_PORT, MAX_CONNECTIONS);

    std::thread serverThread([&server]() {
        server.run();
    });

    // variables
    Trie<Request*> requests;
    ThreadPool pool(MAX_THREAD_COUNT);
    Trie<std::function<void(void*)>> router;

    router.add("/", [](void* data) {
        RouteData *routeData = (RouteData*)data;
        Response *response = routeData->response;
        response->setBody("Hello World!\n");
        response->setHeader("Connection", "close");
        response->setStatus(200);
        response->setStatusText("OK");
    });

    while (true) {
        int numberOfEvents = epoll_wait(epollFd, events, MAX_EPOLL_EVENTS, -1);

        if (numberOfEvents == -1) {
            logError("Error waiting for events");
            return 1;
        }

        for (int i = 0; i < numberOfEvents; i++) {
            int fd = events[i].data.fd;

            if (events[i].events & EPOLLIN) {
                char buff[1024];
                int bytesRead = read(fd, buff, 1024);
                char strnum[12];
                sprintf(strnum, "%d", fd);

                Request* request = requests.get(strnum);
                if (!request) {
                    request = new Request();
                    requests.add(strnum, request);
                }
                request->parse(std::string(buff, bytesRead));
                continue;
            }
            
            if (events[i].events & EPOLLOUT) {
                char strnum[12];
                sprintf(strnum, "%d", fd);
                Request* request = requests.get(strnum);
                if (!request) {
                    continue;
                }
                TransportData* transportData = new TransportData();
                transportData->request = request;
                transportData->fd = fd;
                transportData->router = &router;
                transportData->eventFlags = events[i].events;
                transportData->event = &events[i];
                transportData->server = &server;
                transportData->epollFd = epollFd;
                events[i].events = 0;
                events[i].data.fd = fd;
                epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &events[i]);
                pool.add(handleRequest, transportData);
            }
        }
    }

    serverThread.join();

    return 0;
}