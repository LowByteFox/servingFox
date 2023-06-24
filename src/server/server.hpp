#ifndef SERVER_HPP
#define SERVER_HPP

#include <atomic>

class Server {
    private:
        int epollFd;
        std::atomic_int epollCount;
        int port;
        int maxConnections;
        void addEpollEvent(int fd);

    public:
        Server(int fd, int targetPort, int connections) : epollFd(fd), port(targetPort), maxConnections(connections) {};
        void run();
        void increaseEpollCount()
        {
            epollCount++;
        };
        void decreaseEpollCount()
        {
            epollCount--;
        };
};

#endif