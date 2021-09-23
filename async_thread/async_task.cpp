#include "async_task.h"
#include "utils.h"

void AsyncTask::StartTask(std::promise<TaskContent>& promiseObj, std::function<void(std::shared_ptr<AsyncTask> taskObj)> func, std::shared_ptr<AsyncTask> taskObj)
{
    m_future = promiseObj.get_future();
    std::thread t(&AsyncTask::Task1, this, std::ref(promiseObj));
    t.detach();

    std::thread t1(func, taskObj);
    t1.detach();
}

void AsyncTask::Task1(std::promise<TaskContent>& promiseObj)
{
    PRINT_INFO("start task1...\n");
    promiseObj.set_value({1, "id 1"});
}