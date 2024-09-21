#include "ThreadBondScheduler/Scheduler.h"

SchedulerImpl1::SchedulerImpl1() : m_nextTaskID(0) {}

SchedulerImpl1::~SchedulerImpl1() {}

int SchedulerImpl1::schedule(const Task& task) {
    std::lock_guard<std::mutex> lock(m_mutex);
    int taskID = m_nextTaskID++;
    m_tasks[taskID] = task;
    return taskID;
}

void SchedulerImpl1::unschedule(int taskID) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_tasks.find(taskID);
    if (it != m_tasks.end()) {
        m_tasks.erase(it);
    }
    // 如果taskID不存在，什么也不做
}

int SchedulerImpl1::getTaskCount() const {
    // std::lock_guard<std::mutex> lock(m_mutex);
    return m_tasks.size();
}

void SchedulerImpl1::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_tasks.clear();
}

void SchedulerImpl1::runTasks() {
    std::vector<Task> tasksToRun;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& pair : m_tasks) {
            tasksToRun.push_back(pair.second);
        }
    }
    
    for (auto& task : tasksToRun) {
        task.addExceptionHandler([](const TaskExceptionContext& context) {
            std::cerr << "Task exception: " << context.what() << std::endl;
        });
        task.execute();
    }
}