#include <thread>
#include <chrono>
#include <future>
#include "utils.h"

void thread1(std::promise<int>* promiseObj)
{
    PRINT_INFO("start thread1...\n");
    std::this_thread::sleep_for(std::chrono::seconds(3));
    promiseObj->set_value(1);
}

void SyncWait()
{
    std::promise<int> promiseObj;
    std::future<int> futureObj = promiseObj.get_future();
    std::thread t(thread1, &promiseObj);
    PRINT_INFO("continue in SyncWait after thread1...\n");

    auto result = futureObj.get();
    PRINT_INFO("get result form thread1 in SyncWait: %d\n", result);
    t.join();
}

void thread2(std::future<int>* futureObj)
{
    PRINT_INFO("start thread2...\n");
    auto result = futureObj->get();
    PRINT_INFO("get result form thread1 in thread2: %d\n", result);
}

void AsyncWait()
{
    std::promise<int> promiseObj;
    std::future<int> futureObj = promiseObj.get_future();
    std::thread t1(thread1, &promiseObj);
    PRINT_INFO("continue in AsyncWait after thread1...\n");

    std::thread t2(thread2, &futureObj);
    PRINT_INFO("continue in AsyncWait after thread2...\n");
    t1.join();
    t2.join();
}


int main()
{
    SyncWait();
    AsyncWait();

    return 0;
}