#include "config.h"
#include "server/server.hpp"

#include <map>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/epoll.h>
#include <thread>
#include <trie.hpp>

extern "C" {
#include <crashtrace.h>
#include <logger.h>
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

    while (true) {
        int numberOfEvents = epoll_wait(epollFd, events, MAX_EPOLL_EVENTS, -1);

        if (numberOfEvents == -1) {
            logError("Error waiting for events");
            return 1;
        }

        for (int i = 0; i < numberOfEvents; i++) {
            const epoll_event& event = events[i];
            // handle events
        }
    }

    serverThread.join();

    return 0;
}