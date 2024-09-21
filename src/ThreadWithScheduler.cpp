#include <iostream>
#include <thread>
#include <chrono>
#include <functional>

#include "SingleThreadScheduler/Scheduler.h"
#include "SingleThreadScheduler/ThreadWithScheduler.h"

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
        }
    }

ThreadWithScheduler::ThreadWithScheduler() : m_paused(true), m_stopped(false), m_loopTimeMS(10)
{
    m_thread = new std::thread(&ThreadWithScheduler::run, this);
    m_threadID = m_thread->get_id();
    m_scheduler = new SingleThreadSchedulerImpl1();
}
ThreadWithScheduler::~ThreadWithScheduler()
{

    if (m_thread != nullptr)
    {
        this->pause();
        m_scheduler->stop();
        this->stopAndJoin(); // 这里会等待 m_thread 结束
        delete m_scheduler;
        m_scheduler = nullptr;
        delete m_thread;
        m_thread = nullptr;
    }
}
void ThreadWithScheduler::run()
{
    auto updateFunc = [this]()
    {
        if (!m_paused)
        {
            // m_paused 初始值是 true，所以第一次进入这个循环时，可以保证不会访问到空指针。
            m_scheduler->update();
        }
        /**/ };
    auto finishCheckFunc = [this]()
    { return m_stopped; };
    UpdateLoop(updateFunc, finishCheckFunc, m_loopTimeMS);

    m_scheduler->stop();
}
ISingleThreadScheduler *ThreadWithScheduler::getScheduler()
{
    return m_scheduler;
}

void ThreadWithScheduler::start()
{
    m_paused = false;
    m_stopped = false;
}
void ThreadWithScheduler::stopAndJoin()
{
    // auto logger = spdlog::get("raven");
    // logger->debug("before stop");
    // 如果线程已经标记为停止，则不执行任何操作
    if (m_stopped)
    {
        // 如果 m_stopped 已经是 true，exchange 方法会返回 true，因此直接返回
        return;
    }
    m_stopped = true;
    // logger->debug("after stop");
    // 现在 m_stopped 被设置为 true，并且我们知道这是首次调用 stop

    // 等待线程结束，如果它是可加入的
    this->join();
    // logger->debug("after join");
    // 可以在这里执行任何其他清理工作
}
void ThreadWithScheduler::pause()
{
    // TODO ISingleThreadScheduler 是不是也应该提供 pause 和 resume 的接口？
    // 这样可以更快速的中断网络线程的任务调度。否则要等待这一次任务调度把所有队列中的任务都执行结束。
    m_paused = true;
}
void ThreadWithScheduler::resume()
{
    m_paused = false;
}
void ThreadWithScheduler::setLoopTimeMS(int ms)
{
    m_loopTimeMS = ms;
}
void ThreadWithScheduler::join()
{
    if (m_threadID != std::this_thread::get_id() && m_thread != NULL && m_thread->joinable())
    {
        m_thread->join();
    }
}
