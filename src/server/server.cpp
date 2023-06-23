#include "server.hpp"

extern "C" {
#include <logger.h>
}

#include <cerrno>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

void Server::run()
{
    int serverFd, clientFd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((serverFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == 0) {
        logError("Error creating socket");
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(this->port);

    if (bind(serverFd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        logError("Error binding socket");
        return;
    }

    if (listen(serverFd, this->maxConnections) < 0) {
        logError("Error listening on socket");
        return;
    }

    while (true) {
        clientFd = accept4(serverFd, (struct sockaddr*)&address, (socklen_t*)&addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
        if (clientFd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            } else {
                logError("Error accepting connection");
                return;
            }
        }

        if (this->epollCount >= this->maxConnections) {
            logWarn("Max connections reached");
            close(clientFd);
            continue;
        } {
            this->addEpollEvent(clientFd);
        }
    }
}

void Server::addEpollEvent(int fd)
{
    struct epoll_event* event = new struct epoll_event;
    event->events = EPOLLIN | EPOLLOUT | EPOLLRDHUP;
    event->data.fd = fd;

    if (epoll_ctl(this->epollFd, EPOLL_CTL_ADD, fd, event) == -1) {
        logError("Error adding epoll event");
        return;
    }

    this->increaseEpollCount();
}