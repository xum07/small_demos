#include "thread_pool/thread_pool.h"
#include <gtest/gtest.h>
#include "thread_pool/thread_manager.h"

TEST(thread_pool_test, exec_thread_ok)
{
    constexpr uint32_t ADD_TASK_SPEED = 2;
    auto threadPool = std::make_unique<ThreadPool>(2, 2);
    threadPool->Init();

    for (uint32_t i = 0; i < 20; i++) {
        std::cout << "this is number:" << i << std::endl;
        auto f = threadPool->AddTask(
            [](uint32_t num) {
                sleep(3 * ADD_TASK_SPEED + ADD_TASK_SPEED / 2);
                std::cout << "number(" << num << ") exec finish." << std::endl;
            },
            i);
        sleep(ADD_TASK_SPEED);
    }

    threadPool->Destroy();
}

TEST(thread_pool_test, thread_manager_exec_ok)
{
    std::vector<int> clients = {1, 2, 3, 4, 5, 6};
    auto threadManager = std::make_unique<ThreadManager>(std::make_unique<ThreadPool>(2, 2));
    auto result = threadManager->ParallelInvoke(clients, [](int val) {
        sleep(3);
        std::cout << "client(" << val << ") start exec" << std::endl;
        return val;
    });
}
