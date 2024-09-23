#include "SingleThreadScheduler/Scheduler.h"
using namespace singlethreadscheduler;
SingleThreadSchedulerImpl1::SingleThreadSchedulerImpl1(std::thread::id tid) {
    m_onceScheduler = new SchedulerImpl1();
    m_repeatScheduler = new SchedulerImpl1();
    m_bindingThreadID = tid;
}

SingleThreadSchedulerImpl1::~SingleThreadSchedulerImpl1() {
    //if (std::this_thread::get_id() != m_bindingThreadID) {
    //    throw std::runtime_error("bug here, SingleThreadSchedulerImpl1 destructor should only be called from its binding thread.");
    //}
    if (m_onceScheduler) {
        delete m_onceScheduler;
        m_onceScheduler = nullptr;
    }
    if (m_repeatScheduler) {
        delete m_repeatScheduler;
        m_repeatScheduler = nullptr;
    }
}

int SingleThreadSchedulerImpl1::scheduleOnce(const Task &task) {
    return m_onceScheduler->schedule(task);
}

int SingleThreadSchedulerImpl1::scheduleRepeat(const Task &task) {
    if (!m_isRunning) {
        return -1;
    }
    return m_repeatScheduler->schedule(task);
}

int SingleThreadSchedulerImpl1::unscheduleRepeat(int taskID) {
    if (!m_isRunning) {
        return -1;
    }
    return m_repeatScheduler->unschedule(taskID);
}

void SingleThreadSchedulerImpl1::stop() {
    m_isRunning = false;
    m_repeatScheduler->stop();
    m_onceScheduler->stop();
    // TODO 如果任务线程A正在执行 m_onceScheduler->runTasks 或 m_repeatScheduler->runTasks
    // 从另外一个线程B 调用调用本函数，此时只会修改scheduler的运行标志位
    // 任务线程A会执行完当前正在运行的task之后，检查运行标志位，
}

void SingleThreadSchedulerImpl1::update() {
    if (std::this_thread::get_id() != m_bindingThreadID) {
        throw std::runtime_error("bug here, SingleThreadSchedulerImpl1::update should only be called from its binding thread.");
    }
    while (m_isRunning && m_onceScheduler->getTaskCount() > 0) {
        m_onceScheduler->runTasks(false);
    }
    if (m_isRunning) {
        m_repeatScheduler->runTasks(true);
    }
    // todo 统计一次 update 的执行时间
    // 记录 slow log
}