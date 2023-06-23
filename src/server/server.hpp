#ifndef SERVER_HPP
#define SERVER_HPP

class Server {
    private:
        int epollFd;
        int epollCount;
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