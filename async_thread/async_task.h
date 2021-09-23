#ifndef ASYNC_TASK_H
#define ASYNC_TASK_H

#include <future>
#include <functional>
#include <string>
#include <memory>

class AsyncTask {
public:
    struct TaskContent {
        int id;
        std::string name;
    };

    AsyncTask() = default;
    ~AsyncTask() = default;

    void StartTask(std::promise<TaskContent>& promiseObj, std::function<void(std::shared_ptr<AsyncTask> taskObj)> func, std::shared_ptr<AsyncTask> taskObj);
    std::future<TaskContent> GetFuture() { return std::move(m_future); }

private:
    void Task1(std::promise<TaskContent>& promiseObj);

private:
    std::future<TaskContent> m_future;
};

#endif // ASYNC_TASK_H