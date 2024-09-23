#include <iostream>
#include <thread>
#include <chrono>
#include <functional>

#include "SingleThreadScheduler/Scheduler.h"
#include "SingleThreadScheduler/ThreadWithScheduler.h"
using namespace singlethreadscheduler;
inline void UpdateLoop(std::function<void()> update, std::function<bool()> finishCheck, std::int64_t updateIntervalMS)
{
    std::chrono::steady_clock::time_point lastTickTime = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point lastRealTickTime = lastTickTime;
    while (true)
    {
        update();
        if (finishCheck && finishCheck())
        {
            break;
        }
        if (updateIntervalMS > 0)
        {
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTickTime).count();
            lastTickTime = lastTickTime + std::chrono::milliseconds(updateIntervalMS);
            if (elapsed >= 0 && elapsed < updateIntervalMS)
            {
                auto sleepTimeMS = updateIntervalMS - elapsed;
                // Poco::Thread::sleep((long)sleepTimeMS);
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMS));
                lastRealTickTime = lastTickTime;
            }
            else
            {
                auto realElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastRealTickTime).count();
                if (realElapsed > updateIntervalMS * 2)
                {
                    // slow loop, log it
                    // auto &logger = Poco::Logger::get("raven");
                    // poco_warning_f2(logger, "slow loop %s ms > %s ms", std::to_string(realElapsed), std::to_string(updateIntervalMS));
                    std::cout << "slow loop " << realElapsed << " ms > " << updateIntervalMS << " ms" << std::endl;
                }
                lastRealTickTime = currentTime;
            }
        }
        else {
            // 用 yiled 是不是可以避免 busy loop？
            std::this_thread::yield();
        }
    }
}

ThreadWithScheduler::ThreadWithScheduler() : m_started(false), m_stopped(false), m_loopTimeMS(10)
{
    // 这里会立刻在新线程开始执行 ThreadWithScheduler::run
    // 但因为 m_started = false，所以会在线程内部 sleep 等待
    m_thread = new std::thread(&ThreadWithScheduler::run, this);
    m_threadID = m_thread->get_id();
    m_scheduler = new SingleThreadSchedulerImpl1(m_threadID);
    m_beforeStopScheduler = new SchedulerImpl1();
}


int ThreadWithScheduler::scheduleTaskBeforeThreadStop(const Task &task) {
    if (m_stopped) {
        return -1;
    }
    if (m_beforeStopScheduler == nullptr) {
        return -2;
    }
    return m_beforeStopScheduler->schedule(task);
}
void ThreadWithScheduler::run()
{
    auto updateFunc = [this]()
        {
            if (m_started)
            {
                // m_paused 初始值是 true，所以第一次进入这个循环时，可以保证不会访问到空指针。
                m_scheduler->update();
            }
            /**/ };
    auto finishCheckFunc = [this]()
        { return m_stopped; };
    UpdateLoop(updateFunc, finishCheckFunc, m_loopTimeMS);

    // 只有当 m_stopped = true 是，上面的 UpdateLoop 才会结束。
    // 此时，执行所有通过 ThreadWithScheduler::scheduleTaskBeforeThreadStop 调度的任务
    if (m_beforeStopScheduler != nullptr) {
        m_beforeStopScheduler->runTasks(false);
        delete m_beforeStopScheduler;
        m_beforeStopScheduler = nullptr;
    }
}
ISingleThreadScheduler *ThreadWithScheduler::getScheduler()
{
    return m_scheduler;
}

void ThreadWithScheduler::start()
{
    m_started = true;
}

void ThreadWithScheduler::setLoopTimeMS(int ms)
{
    m_loopTimeMS = ms;
}
void ThreadWithScheduler::stopAndJoin()
{
    // 是不是要考虑从其他线程调用该方法，或从 m_thread 调用该方法两种情况？
    if (m_stopped)
    {
        return;
    }
    m_stopped = true;
    m_scheduler->stop();
    if (m_threadID != std::this_thread::get_id())
    {
        // TODO 是否需要检查 m_thread != NULL && m_thread->joinable()
        m_thread->join();
        // 等待 m_thread 执行并退出 ThreadWithScheduler::run 函数
    }
    else {
        // 在 m_thread 内部调用 stopAndJoin，此时可以直接执行 tasksBeforeStop
        m_beforeStopScheduler->runTasks(false);
        delete m_beforeStopScheduler;
        m_beforeStopScheduler = nullptr;
    }

    delete m_scheduler;
    m_scheduler = nullptr;
    // m_beforeStopScheduler 应该已经是 nullptr 的状态。
    delete m_thread;
    m_thread = nullptr;
}
ThreadWithScheduler::~ThreadWithScheduler()
{
    this->stopAndJoin();
}