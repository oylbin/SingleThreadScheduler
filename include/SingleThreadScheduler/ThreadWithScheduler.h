#pragma once

#include <thread>
#include <atomic>
#include <mutex>

#include "SingleThreadScheduler/Scheduler.h"

// 带调度器和带主循环的线程对象
class ThreadWithScheduler
{
public:
    ThreadWithScheduler();

    ISingleThreadScheduler *getScheduler();

    // 禁止拷贝构造和拷贝赋值
    ThreadWithScheduler(const ThreadWithScheduler &) = delete;
    ThreadWithScheduler &operator=(const ThreadWithScheduler &) = delete;
    // 禁止移动构造和移动赋值
    ThreadWithScheduler(ThreadWithScheduler &&) = delete;
    ThreadWithScheduler &operator=(ThreadWithScheduler &&) = delete;

    virtual ~ThreadWithScheduler();
    void start();
    void stopAndJoin();
    void pause();
    void resume();
    void join();
    virtual void run();
    void setLoopTimeMS(int ms);

private:
    bool m_paused;
    bool m_stopped;
    std::int64_t m_loopTimeMS;
    std::thread *m_thread;
    std::thread::id m_threadID;
    ISingleThreadScheduler *m_scheduler;
};