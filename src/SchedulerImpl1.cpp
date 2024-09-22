#include "SingleThreadScheduler/Scheduler.h"

using namespace singlethreadscheduler;

SchedulerImpl1::SchedulerImpl1() : m_nextTaskID(0), m_isRunning(true) {}

SchedulerImpl1::~SchedulerImpl1() {}

int SchedulerImpl1::schedule(const Task& task) {
    if (!m_isRunning) {
        return -1;
    }
    std::lock_guard<std::mutex> lock(m_mutex);
    int taskID = m_nextTaskID++;
    m_tasks[taskID] = task;
    return taskID;
}

bool SchedulerImpl1::unschedule(int taskID) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_tasks.find(taskID);
    if (it != m_tasks.end()) {
        m_tasks.erase(it);
        return true;
    }
    return false;
}

int SchedulerImpl1::getTaskCount() const {
    // std::lock_guard<std::mutex> lock(m_mutex);
    return m_tasks.size();
}

void SchedulerImpl1::stop() {
    m_isRunning = false;
}

void SchedulerImpl1::runTasks(bool keepOldTasks) {
    if (!m_isRunning) {
        return;
    }
    std::vector<Task> tasksToRun;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        // 创建一个vector来存储和排序任务ID
        std::vector<int> sortedTaskIds;
        sortedTaskIds.reserve(m_tasks.size());
        for (const auto& pair : m_tasks) {
            sortedTaskIds.push_back(pair.first);
        }
        // 按ID排序
        std::sort(sortedTaskIds.begin(), sortedTaskIds.end());
        // 将排序后的任务添加到tasksToRun
        for (int id : sortedTaskIds) {
            tasksToRun.push_back(m_tasks[id]);
        }
        if (!keepOldTasks) {
            m_tasks.clear();
        }
    }
    
    for (auto& task : tasksToRun) {
        task.addExceptionHandler([](const TaskExceptionContext& context) {
            std::cerr << "Task exception: " << context.what() << std::endl;
        });
        task.execute();
        // TODO 对 task 的排队时间和执行时间进行统计
    }
}