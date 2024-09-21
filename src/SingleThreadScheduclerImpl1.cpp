#include "SingleThreadScheduler/Scheduler.h"

SingleThreadSchedulerImpl1::SingleThreadSchedulerImpl1() {
    m_onceScheduler = new SchedulerImpl1();
    m_repeatScheduler = new SchedulerImpl1();
    m_taskBeforeStopScheduler = new SchedulerImpl1();
}

SingleThreadSchedulerImpl1::~SingleThreadSchedulerImpl1() {
    if (m_onceScheduler) {
        delete m_onceScheduler;
        m_onceScheduler = nullptr;
    }
    if (m_repeatScheduler) {
        delete m_repeatScheduler;
        m_repeatScheduler = nullptr;
    }
    if (m_taskBeforeStopScheduler) {
        delete m_taskBeforeStopScheduler;
        m_taskBeforeStopScheduler = nullptr;
    }
}

int SingleThreadSchedulerImpl1::scheduleOnce(const Task& task) {
    return m_onceScheduler->schedule(task);
}

int SingleThreadSchedulerImpl1::scheduleRepeat(const Task& task) {
    return m_repeatScheduler->schedule(task);
}

int SingleThreadSchedulerImpl1::unscheduleRepeat(int taskID) {
    return m_repeatScheduler->unschedule(taskID);
}

int SingleThreadSchedulerImpl1::scheduleTaskBeforeStop(const Task& task) {
    return m_taskBeforeStopScheduler->schedule(task);
}

void SingleThreadSchedulerImpl1::stop() {
    m_isRunning = false;
    m_repeatScheduler->stop();
    m_onceScheduler->stop();
    m_taskBeforeStopScheduler->runTasks(false);
    m_taskBeforeStopScheduler->stop();
}

void SingleThreadSchedulerImpl1::update() {
    while(m_onceScheduler->getTaskCount() > 0) {
        m_onceScheduler->runTasks(false);
    }
    m_repeatScheduler->runTasks(true);
    // todo 统计一次 update 的执行时间
    // 记录 slow log
}