#include "thread_pool.h"

void ThreadPool::Init()
{
    // construct a thread object which is in the loop of waiting notification
    auto th = [this]() {
        while (_poolStat == PoolStat::RUNNING) {
            Task task;
            {
                /**
                 * condition_variable will not block or be waked up under:
                 * 1. task queue is not empty
                 * 2. thread pool is going to stop
                 */
                std::unique_lock<std::mutex> lock {_waitQueLock};
                this->_threadCV.wait(lock,
                                     [this]() { return !this->_waitQue.empty() || (_poolStat == PoolStat::STOP); });

                // additional judge to accelerate stopping of pool
                if (_poolStat == PoolStat::STOP) {
                    return;
                }

                task = std::move(this->_waitQue.front());
                _waitQue.pop();
                _waitQueFreeSize++;
            }

            task();
        }
    };

    // fill _threads with wait loop function
    for (uint32_t i = 0; i < _threads.capacity(); i++) {
        _threads.emplace_back(th);
    }
}

void ThreadPool::Destroy()
{
    std::cout << "ThreadPool is going to stop!" << std::endl;
    _poolStat.store(PoolStat::STOP);
    _threadCV.notify_all();

    for (auto& th : _threads) {
        if (th.joinable()) {
            th.join();
        }
    }
}