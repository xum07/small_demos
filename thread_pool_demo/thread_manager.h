#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <memory>
#include "thread_pool.h"

class ThreadManager {
public:
    ThreadManager(std::unique_ptr<ThreadPool> threadPool)
    {
        _threadPool = std::move(threadPool);
    }
    ThreadManager()
    {
        _threadPool = std::make_unique<ThreadPool>(MAX_THREAD_NUMBER, MAX_TASK_QUEUE_SIZE);
    };

    virtual ~ThreadManager() = default;

    template <typename _Container, typename _Function>
    auto ParallelInvoke(const _Container &funcArgs, _Function func)
        -> std::vector<decltype(func(std::declval<typename _Container::value_type>()))>;

private:
    std::unique_ptr<ThreadPool> _threadPool;
};

template <typename _Container, typename _Functor>
auto ThreadManager::ParallelInvoke(const _Container &funcArgs, _Functor func)
    -> std::vector<decltype(func(std::declval<typename _Container::value_type>()))>
{
    using ResType = decltype(func(std::declval<typename _Container::value_type>()));

    std::vector<ResType> result;
    std::vector<std::future<ResType> > futures;
    for (const auto &arg : funcArgs) {
        futures.emplace_back(_threadPool->AddTask(func, arg));
    }

    std::for_each(futures.begin(), futures.end(), [&result](auto &f) {
        if (f.valid()) {
            result.emplace_back(f.get());
        }
    });
    return std::move(result);
}

#endif  // THREAD_MANAGER_H