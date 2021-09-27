#include <iostream>
#include "async_task.h"
#include "utils.h"

void AsyncTask::StartTask(std::function<void(TaskContent&)> func)
{
    auto promiseObj = std::promise<TaskContent>();
    auto fut(promiseObj.get_future());
    std::thread t(&AsyncTask::Task1, this, std::ref(promiseObj));
    t.detach();

    m_future = std::move(fut);
    std::thread t1([this, &func](){
        auto result = m_future.get();
        func(result);
    });
    t1.detach();
}

void AsyncTask::Task1(std::promise<TaskContent>& promiseObj)
{
    PRINT_INFO("start task1...\n");
    promiseObj.set_value({1, "id 1"});
}