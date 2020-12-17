#include <memory>
#include <unistd.h>
#include "thread_pool.h"
#include "utils.h"

constexpr uint32_t ADD_TASK_SPEED = 2;

void funA(uint32_t index)
{
    //control speed to test task queue and thread queue both
    sleep(3 * ADD_TASK_SPEED + ADD_TASK_SPEED/2);
    PRINT_INFO("number %d exec finish.", index);
}

int main()
{
    auto threadPool = std::make_unique<ThreadPool>(2, 2);
    for(uint32_t i=0; i < 20; i++) {
        PRINT_INFO("this is number %d.", i);
        auto f = threadPool->AddTask(funA, i);
        sleep(ADD_TASK_SPEED);
    }

    threadPool.reset();
    return 0;
}