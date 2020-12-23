#include <memory>
#include <unistd.h>
#include "thread_pool.h"
#include "thread_manager.h"
#include "utils.h"

int main()
{
    std::vector<int> clients = {1, 2, 3, 4, 5, 6};
    auto threadManager = std::make_unique<ThreadManager>(std::make_unique<ThreadPool>(2, 2));
    auto ret = threadManager->ParallelInvoke(clients, [](const int& val) {
        sleep(3);
        PRINT_INFO("client(%d) finish.", val);

        return val;
    });

    threadManager.reset();
    return 0;
}