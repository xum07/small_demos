#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <functional>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include "utils.h"

constexpr uint32_t MAX_THREAD_NUMBER = 10;
constexpr uint32_t MAX_TASK_QUEUE_SIZE = 100;

class ThreadPool {
public:
    using Task = std::function<void()>;

    ThreadPool(uint32_t threadNum = 1, uint32_t taskQueueSize = 1);
    ~ThreadPool();

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool(const ThreadPool &&) = delete;
    ThreadPool &operator=(const ThreadPool &&) = delete;

    template<typename F, typename... Args>
    auto AddTask(F&& f, Args&&... args) -> std::future<decltype(f(args...))>;

private:
    // avoid to use std::atomic<bool> or std::atomic<uint64_t>, because they are not supportted well in ARM
    std::atomic<uint32_t> _isRunning {1};
    std::queue<Task> _taskQueue;
    std::atomic<uint32_t> _taskQueueFreeSize;
    std::mutex _taskQueueLock;
    std::condition_variable _taskCV;
    std::vector<std::thread> _threads;
};

ThreadPool::ThreadPool(uint32_t threadNum, uint32_t taskQueueSize)
{
    _taskQueueFreeSize = (taskQueueSize < 1) ? 1 : ((taskQueueSize > MAX_TASK_QUEUE_SIZE) ? MAX_TASK_QUEUE_SIZE : taskQueueSize);
    uint32_t realThreadNum = (threadNum < 1) ? 1 : ((threadNum > MAX_THREAD_NUMBER) ? MAX_THREAD_NUMBER : threadNum);

    // construct a thread object which is in the loop of waiting notification and execution
    auto th = [this]() {
        while (_isRunning.load() == 1) {
            Task task;
            {
                std::unique_lock<std::mutex> lock{_taskQueueLock};
                this->_taskCV.wait(lock, [this]() { return  !this->_taskQueue.empty() || (_isRunning.load() == 0); });

                if ((_isRunning.load() == 0) || this->_taskQueue.empty()) {
                    return;
                }

                task = std::move(this->_taskQueue.front());
                _taskQueue.pop();
                _taskQueueFreeSize++;
            }

            task();
        }
    };

    for (uint32_t i=0; i<realThreadNum; i++) {
        _threads.emplace_back(th);
    }
}

ThreadPool::~ThreadPool()
{
    PRINT_INFO("ThreadPool is going to stop!");
    _isRunning.store(0);
    _taskCV.notify_all();

    for(std::thread &th : _threads) {
        if(th.joinable()) {
            th.join();
        }
    }
}

template<typename F, typename... Args>
auto ThreadPool::AddTask(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
{
    using FuncType = decltype(f(args...));

    if (_isRunning.load() == 0) {
        PRINT_INFO("ThreadPool is not running.");
        return std::future<FuncType>();
    }

    std::lock_guard<std::mutex> lock{_taskQueueLock};
    if (_taskQueueFreeSize == 0) {
        PRINT_INFO("TaskQueue is full, can not add any more!");
        return std::future<FuncType>();
    }

    auto task = std::make_shared<std::packaged_task<FuncType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<FuncType> result = task->get_future();
    {
        _taskQueue.emplace([task]() { (*task)(); });
        _taskQueueFreeSize--;
        _taskCV.notify_one();
    }

    return result;
}

#endif // THREAD_POOL_H