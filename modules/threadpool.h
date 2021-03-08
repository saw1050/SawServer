#pragma once

#include<thread>
#include<functional>
#include<queue>
#include<mutex>
#include<condition_variable>
#include<atomic>
#include<cstring>

constexpr std::memory_order MEM_ORD = std::memory_order_relaxed;

class ThreadPool
{
    typedef std::function<void()> Task;
private:
    std::queue<Task> tasks_;
    mutable std::mutex mutex_;
    mutable std::mutex rod_;
    std::condition_variable condition_;
    std::condition_variable ctrl_;
    std::atomic<bool> isWorking_;
    std::atomic<bool> fireOne_;
    std::atomic<uint32_t> allThreadsCount_;         // 线程总数
    std::atomic<uint32_t> idleThreadsCount_;        // 闲置线程数
    std::atomic<uint32_t> tasksCount_;

public:
    ThreadPool();
    ~ThreadPool();

    void execute(Task task);
    void start();
    void stop();
    void interrupt();
    void wait();
    void awake();

    uint32_t getThreadsCount() const;
    uint32_t getTasksCount() const;
    uint32_t getIdleCount() const;

    void newThreads(uint32_t count);
    void newThread();
    void closeThreads(uint32_t count);

private:
    void task();
};