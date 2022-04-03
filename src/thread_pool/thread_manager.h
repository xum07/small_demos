#ifndef SMALL_DEMOS_THREAD_MANAGER_H
#define SMALL_DEMOS_THREAD_MANAGER_H

#include <future>
#include <memory>
#include <vector>
#include "thread_pool.h"

class ThreadManager {
public:
    explicit ThreadManager(std::unique_ptr<ThreadPool> threadPool)
    {
        _threadPool = std::move(threadPool);
        _threadPool->Init();
    }
    ~ThreadManager()
    {
        _threadPool->Destroy();
    }

    template<typename Container, typename Func>
    auto ParallelInvoke(const Container& funcArgs, Func func)
        -> std::vector<decltype(func(std::declval<typename Container::value_type>()))>;

private:
    std::unique_ptr<ThreadPool> _threadPool;
};

template<typename Container, typename Func>
auto ThreadManager::ParallelInvoke(const Container& funcArgs, Func func)
    -> std::vector<decltype(func(std::declval<typename Container::value_type>()))>
{
    using ResType = decltype(func(std::declval<typename Container::value_type>()));

    std::vector<ResType> result;
    std::vector<std::future<ResType>> futures;
    for (const auto& arg : funcArgs) {
        futures.emplace_back(_threadPool->AddTask(func, arg));
    }

    std::for_each(futures.begin(), futures.end(), [&result](auto& f) {
        if (f.valid()) {
            result.emplace_back(f.get());
        }
    });

    return result;
}

#endif  // SMALL_DEMOS_THREAD_MANAGER_H