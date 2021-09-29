#include <thread>
#include <chrono>
#include <future>
#include <iostream>
#include "utils.h"
#include "async_task.h"

void thread1(std::promise<int>* promiseObj)
{
    PRINT_INFO("start thread1...\n");
    std::this_thread::sleep_for(std::chrono::seconds(3));
    promiseObj->set_value(3);
}

void SyncWait()
{
    std::promise<int> promiseObj;
    std::future<int> futureObj = promiseObj.get_future();
    std::thread t(thread1, &promiseObj);
    t.detach();
    PRINT_INFO("continue in SyncWait after thread1...\n");

    auto result = futureObj.get();
    PRINT_INFO("get result form thread1 in SyncWait: %d\n", result);
}

void AsyncWait(AsyncTask::TaskContent& result)
{
    std::cout << "result id is: " << result.id << ", name is: " << result.name << std::endl;
}

int main()
{
    // sync test
    SyncWait();
    
    // async test
    auto taskObj = std::make_unique<AsyncTask>();
    auto promiseObj = std::promise<AsyncTask::TaskContent>();
    taskObj->StartTask(std::move(promiseObj), AsyncWait);

    std::this_thread::sleep_for(std::chrono::seconds(10));
    PRINT_INFO("end main\n");
    return 0;
}