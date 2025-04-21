#pragma once

#include "ThreadPool.hpp"
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>

namespace Common {

class TaskManager {
public:
    TaskManager()
        : m_ThreadPool(std::make_unique<ThreadPool>()) { }

    // Start the farming bot task
    void StartFarmBot(const std::function<void()>& farmingFunction,
                      std::chrono::milliseconds interval = std::chrono::milliseconds(1000)) {
        m_ThreadPool->ScheduleRecurring(farmingFunction, interval);
    }

    // Schedule a one-time task
    template <typename F, typename... Args> auto ScheduleTask(F&& f, Args&&... args) {
        return m_ThreadPool->EnqueueTask(std::forward<F>(f), std::forward<Args>(args)...);
    }

    // Schedule a recurring task
    template <typename F> void ScheduleRecurringTask(F&& f, std::chrono::milliseconds interval) {
        m_ThreadPool->ScheduleRecurring(std::forward<F>(f), interval);
    }

private:
    std::unique_ptr<ThreadPool> m_ThreadPool;
};

} // namespace Common