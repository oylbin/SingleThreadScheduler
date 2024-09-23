#pragma once

#include <thread>
#include <atomic>
#include <mutex>

#include "SingleThreadScheduler/Scheduler.h"

namespace singlethreadscheduler {

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

        ~ThreadWithScheduler();
        int scheduleTaskBeforeThreadStop(const Task &task);
        void start();
        void stopAndJoin();

        void setLoopTimeMS(int ms);

    private:
        void run();
        bool m_started{ false };
        bool m_stopped{ false };
        std::int64_t m_loopTimeMS{ 10 };
        std::thread *m_thread{ nullptr };
        std::thread::id m_threadID;
        ISingleThreadScheduler *m_scheduler{ nullptr };
        IScheduler *m_beforeStopScheduler{ nullptr };
    };

} // namespace singlethreadscheduler
