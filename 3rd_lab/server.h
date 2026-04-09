#pragma once

#include <queue>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <cstddef>

template<typename T>
class Server
{
private:
    struct Task
    {
        size_t id;
        std::function<T()> func;
    };

    std::queue<Task> task_queue_;

    std::unordered_map<size_t, T> results_;

    std::mutex queue_mutex_;
    std::mutex results_mutex_;

    std::condition_variable queue_cv_;
    std::condition_variable results_cv_;

    std::jthread worker_;

    std::atomic<size_t> next_id_{0};

    void run(std::stop_token stoken)
    {
        while (true)
        {
            Task task;
            {
                std::unique_lock lock(queue_mutex_);
                queue_cv_.wait(lock, [&]
                {
                    return !task_queue_.empty() || stoken.stop_requested();
                });

                if (task_queue_.empty())
                {
                    if (stoken.stop_requested())
                        break;
                    continue;
                }

                task = std::move(task_queue_.front());
                task_queue_.pop();
            }

            T result = task.func();

            {
                std::lock_guard lock(results_mutex_);
                results_[task.id] = std::move(result);
            }
            results_cv_.notify_all();
        }
    }

public:
    Server() = default;

    ~Server()
    {
        if (worker_.joinable())
            stop();
    }

    void start()
    {
        worker_ = std::jthread([this](std::stop_token st) { run(st); });
    }

    void stop()
    {
        worker_.request_stop();
        queue_cv_.notify_all();
        if (worker_.joinable())
            worker_.join();
    }

    size_t add_task(std::function<T()> task)
    {
        size_t id = next_id_.fetch_add(1);
        {
            std::lock_guard lock(queue_mutex_);
            task_queue_.push({id, std::move(task)});
        }
        queue_cv_.notify_one();
        return id;
    }

    T request_result(size_t id_res)
    {
        std::unique_lock lock(results_mutex_);
        results_cv_.wait(lock, [&]
        {
            return results_.count(id_res) > 0;
        });
        T result = std::move(results_[id_res]);
        results_.erase(id_res);
        return result;
    }
};
