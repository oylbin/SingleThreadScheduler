#pragma once

#include "Task.h"

// class IScheduler {
// public:
//     virtual void scheduleOnce(const Task& task) = 0;
//     virtual void unscheduleOnce(int taskID) = 0;
//     virtual void scheduleRepeat(const Task& task) = 0;
//     virtual void unscheduleRepeat(int taskID) = 0;
//     virtual void scheduleTaskBeforeStop(const Task& task) = 0;
//     virtual void unscheduleTaskBeforeStop(int taskID) = 0;
//     virtual void runOnceTasks() = 0;
//     virtual void runRepeatTasks() = 0;
//     virtual void runTasksBeforeStop() = 0;
//     virtual void stop() = 0;
// };

class IScheduler {
public:
    virtual int schedule(const Task& task) = 0;
    virtual void unschedule(int taskID) = 0;
    virtual void runTasks() = 0;
    virtual int getTaskCount() const = 0;
    virtual void clear() = 0;
};

class SchedulerImpl1 : public IScheduler {
public:
    SchedulerImpl1();
    ~SchedulerImpl1();
    int schedule(const Task& task) override;
    void unschedule(int taskID) override;
    void runTasks() override;
    int getTaskCount() const override;
    void clear() override;
private:
    std::unordered_map<int, Task> m_tasks;
    std::mutex m_mutex;
    int m_nextTaskID;
};


// class OnceScheduler : public IScheduler {
// public:
//     void schedule(const Task& task) override {
//         // 实现单次任务调度
//     }
//     void unschedule(int taskID) override {
//         // 实现取消单次任务调度
//     }
//     void runScheduledTasks() override {