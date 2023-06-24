#include "pool.hpp"
#include <cstdio>
#include <functional>
#include <mutex>
#include <thread>

ThreadPool::ThreadPool(int threadCount)
{
    for (int i = 0; i < threadCount; i++) {
        std::thread thread(
            [this](void* data) {
                while (true) {
                    if (!this->running) {
                        break;
                    }

                    this->queueMutex.lock();
                    if (this->queue.size() == 0) {
                        this->queueMutex.unlock();
                        continue;
                    }
                    std::function<void(void*)> func = this->popFunc();
                    void* funcData = this->popData();
                    this->queueMutex.unlock();
                    func(funcData);
                }
            },
            nullptr);

        pool.push_back(std::move(thread));
    }
}

void ThreadPool::add(std::function<void(void*)> func, void* data)
{
    queueMutex.lock();
    this->queue.push_back(func);
    this->data.push_back(data);
    queueMutex.unlock();
}

std::function<void(void*)> ThreadPool::popFunc()
{
    std::function<void(void*)> func = this->queue.back();
    this->queue.pop_back();
    return func;
}

void* ThreadPool::popData()
{
    void* data = this->data.back();
    this->data.pop_back();
    return data;
}

void ThreadPool::join()
{
    for (int i = 0; i < this->pool.size(); i++) {
        this->pool[i].join();
    }
}

bool ThreadPool::isDone()
{
    return this->queue.size() == 0;
}

void ThreadPool::stop()
{
    this->queueMutex.lock();
    this->running = false;
    this->queueMutex.unlock();
}