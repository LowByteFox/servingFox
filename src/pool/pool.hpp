#ifndef POOL_HPP
#define POOL_HPP

#include <vector>
#include <functional>
#include <mutex>
#include <thread>

class ThreadPool {
private:
    std::mutex queueMutex;
    std::vector<std::thread> pool;
    std::vector<void*> data;
    std::vector<std::function<void(void*)>> queue;
    bool running = true;

public:
    ThreadPool(int threadCount);
    void add(std::function<void(void*)> func, void* data);
    std::function<void(void*)> popFunc();
    void* popData();
    void join();
    bool isDone();
    void stop();
};
#endif