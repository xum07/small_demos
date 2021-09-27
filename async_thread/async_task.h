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

    void StartTask(std::function<void(TaskContent&)> func);

private:
    void Task1(std::promise<TaskContent>& promiseObj);

private:
    std::future<TaskContent> m_future;
};

#endif // ASYNC_TASK_H