#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <atomic>

namespace Common {

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads = std::thread::hardware_concurrency())
        : m_Stop(false) {
        for (size_t i = 0; i < numThreads; ++i) {
            m_Workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(m_QueueMutex);
                        m_Condition.wait(lock, [this] {
                            return m_Stop || !m_Tasks.empty();
                        });

                        if (m_Stop && m_Tasks.empty()) {
                            return;
                        }

                        task = std::move(m_Tasks.front());
                        m_Tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    template<typename F, typename... Args>
    auto EnqueueTask(F&& f, Args&&... args) 
        -> std::future<typename std::invoke_result<F, Args...>::type> {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);
            if (m_Stop) {
                throw std::runtime_error("Cannot enqueue task on stopped ThreadPool");
            }

            m_Tasks.emplace([task]() { (*task)(); });
        }
        m_Condition.notify_one();
        return res;
    }

    // Schedule a recurring task with a specified interval
    template<typename F>
    void ScheduleRecurring(F&& f, std::chrono::milliseconds interval) {
        auto task = [f = std::forward<F>(f), interval, this]() {
            while (!m_Stop) {
                f();
                std::this_thread::sleep_for(interval);
            }
        };

        EnqueueTask(std::move(task));
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);
            m_Stop = true;
        }
        m_Condition.notify_all();
        for (std::thread& worker : m_Workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

private:
    std::vector<std::thread> m_Workers;
    std::queue<std::function<void()>> m_Tasks;
    std::mutex m_QueueMutex;
    std::condition_variable m_Condition;
    std::atomic<bool> m_Stop;
};

} // namespace Common