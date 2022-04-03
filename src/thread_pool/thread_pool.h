#ifndef SMALL_DEMOS_THREAD_POOL_H
#define SMALL_DEMOS_THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
    using Task = std::function<void()>;

    ThreadPool(uint32_t poolSize, uint32_t waitQueueSize)
    {
        constexpr uint32_t MIN_SIZE = 1;
        constexpr uint32_t MAX_WAIT_QUEUE_SIZE = 10;
        constexpr uint32_t MAX_POOL_SIZE = 10;

        _waitQueFreeSize = (waitQueueSize < MIN_SIZE)
                               ? MIN_SIZE
                               : ((waitQueueSize > MAX_WAIT_QUEUE_SIZE) ? MAX_WAIT_QUEUE_SIZE : waitQueueSize);
        uint32_t realPoolSize = (poolSize < MIN_SIZE) ? MIN_SIZE
                                                      : ((poolSize > MAX_POOL_SIZE) ? MAX_POOL_SIZE : poolSize);
        _threads.reserve(realPoolSize);
    }
    ~ThreadPool() = default;

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(const ThreadPool&&) = delete;
    ThreadPool& operator=(const ThreadPool&&) = delete;

    void Init();
    void Destroy();

    template<typename F, typename... Args>
    auto AddTask(F&& f, Args&&... args) -> std::future<decltype(f(args...))>;

private:
    enum class PoolStat
    {
        RUNNING,
        STOP,
    };
    // avoid to use std::atomic<bool> or std::atomic<uint64_t>, because they are not supportted well in ARM
    std::atomic<PoolStat> _poolStat {PoolStat::RUNNING};
    std::queue<Task> _waitQue;
    std::atomic<uint32_t> _waitQueFreeSize {1};
    std::mutex _waitQueLock;
    std::vector<std::thread> _threads;
    std::condition_variable _threadCV;
};

template<typename F, typename... Args>
auto ThreadPool::AddTask(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
{
    using FuncType = decltype(f(args...));

    if (_poolStat == PoolStat::STOP) {
        std::cout << "ThreadPool is not running!" << std::endl;
        return std::future<FuncType>();
    }

    std::lock_guard<std::mutex> lock {_waitQueLock};
    if (_waitQueFreeSize == 0) {
        std::cout << "TaskQueue is full, can not add any more!" << std::endl;
        return std::future<FuncType>();
    }

    auto task =
        std::make_shared<std::packaged_task<FuncType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<FuncType> result = task->get_future();
    {
        _waitQue.emplace([task]() { (*task)(); });
        _waitQueFreeSize--;
        _threadCV.notify_one();
    }

    return result;
}

#endif  //SMALL_DEMOS_THREAD_POOL_H
