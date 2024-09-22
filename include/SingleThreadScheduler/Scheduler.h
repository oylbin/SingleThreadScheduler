#pragma once

#include "SingleThreadScheduler/Task.h"

namespace singlethreadscheduler {

class IScheduler {
public:
    virtual ~IScheduler() = default;
    // return taskID, return <0 if failed
    virtual int schedule(const Task& task) = 0;
    // return true if success
    virtual bool unschedule(int taskID) = 0;

    // run tasks in order of taskID (which is the order of schedule)
    // task queue is cleared before run, but when running a task, the task may schedule new task.
    // so after runTasks is called, the tasks queue may be not empty,
    // the new task will not be run until next runTasks is called.
    // if keepOldTasks is true, the old tasks will not be cleared.
    virtual void runTasks(bool keepOldTasks = false) = 0;

    virtual int getTaskCount() const = 0;

    // not accept new task after stop
    virtual void stop() = 0;

    // 是否要提供 clear 接口？考虑到多线程的应用场景，感觉clear并没有什么实际意义。
    // 比如一个线程调用 clear，而另外一个现场调用 schedule，那么要么是没有clear成功，要么是clear成功后，新的任务被丢弃而没有执行。
    // virtual void clear() = 0;
};

class SchedulerImpl1 : public IScheduler {
public:
    SchedulerImpl1();
    ~SchedulerImpl1() override;
    int schedule(const Task& task) override;
    bool unschedule(int taskID) override;

    void runTasks(bool keepOldTasks = false) override;
    int getTaskCount() const override;
    void stop() override;
private:
    std::unordered_map<int, Task> m_tasks;
    std::mutex m_mutex;
    int m_nextTaskID{0};
    bool m_isRunning{true};
};


class ISingleThreadScheduler {
public:
    virtual ~ISingleThreadScheduler() = default;
    virtual int scheduleOnce(const Task& task) = 0;
    // virtual bool unscheduleOnce(int taskID) = 0;
    virtual int scheduleRepeat(const Task& task) = 0;
    virtual int unscheduleRepeat(int taskID) = 0;
    virtual int scheduleTaskBeforeStop(const Task& task) = 0;
    // virtual bool unscheduleTaskBeforeStop(int taskID) = 0;
    // virtual void runOnceTasks() = 0;
    // virtual void runRepeatTasks() = 0;
    // virtual void runTasksBeforeStop() = 0;

    // run tasks in order of schedule time
    // run once tasks and then repeat tasks
    virtual void update() = 0;

    // stop scheduler, not accept new task after stop
    // run all the scheduled tasks before stop
    virtual void stop() = 0;
};

class SingleThreadSchedulerImpl1 : public ISingleThreadScheduler {
public:
    SingleThreadSchedulerImpl1();
    ~SingleThreadSchedulerImpl1() override;
    int scheduleOnce(const Task& task) override;
    int scheduleRepeat(const Task& task) override;
    int unscheduleRepeat(int taskID) override;
    int scheduleTaskBeforeStop(const Task& task) override;
    void stop() override;
    void update() override;
private:
    IScheduler* m_onceScheduler;
    IScheduler* m_repeatScheduler;
    IScheduler* m_taskBeforeStopScheduler;
    bool m_isRunning{true};
};

} // namespace singlethreadscheduler
