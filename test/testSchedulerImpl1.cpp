#include <gtest/gtest.h>
#include "SingleThreadScheduler/Scheduler.h"
#include <thread>
#include <atomic>
#include <vector>
using namespace singlethreadscheduler;
class SchedulerImpl1Test : public ::testing::Test {
protected:
    SchedulerImpl1 scheduler;
};

TEST_F(SchedulerImpl1Test, ScheduleTask) {
    Task task([]() { /* 空任务 */ }, __FILE__, __LINE__);
    int taskId = scheduler.schedule(task);
    EXPECT_GE(taskId, 0);
    EXPECT_EQ(scheduler.getTaskCount(), 1);
}

TEST_F(SchedulerImpl1Test, UnscheduleTask) {
    Task task([]() { /* 空任务 */ }, __FILE__, __LINE__);
    int taskId = scheduler.schedule(task);
    EXPECT_EQ(scheduler.getTaskCount(), 1);
    
    scheduler.unschedule(taskId);
    EXPECT_EQ(scheduler.getTaskCount(), 0);
    
    int newTaskId = scheduler.schedule(task);
    EXPECT_NE(taskId, newTaskId);
    EXPECT_EQ(scheduler.getTaskCount(), 1);
}

TEST_F(SchedulerImpl1Test, RunTasks) {
    bool task1Executed = false;
    bool task2Executed = false;
    
    Task task1([&task1Executed]() { task1Executed = true; }, __FILE__, __LINE__);
    Task task2([&task2Executed]() { task2Executed = true; }, __FILE__, __LINE__);
    
    scheduler.schedule(task1);
    scheduler.schedule(task2);
    EXPECT_EQ(scheduler.getTaskCount(), 2);
    
    scheduler.runTasks();
    
    EXPECT_TRUE(task1Executed);
    EXPECT_TRUE(task2Executed);
    EXPECT_EQ(scheduler.getTaskCount(), 0);  // 任务执行后调度器中没有任务
}
TEST_F(SchedulerImpl1Test, RunAndKeepOldTasks) {
    bool task1Executed = false;
    bool task2Executed = false;
    
    Task task1([&task1Executed]() { task1Executed = true; }, __FILE__, __LINE__);
    Task task2([&task2Executed]() { task2Executed = true; }, __FILE__, __LINE__);
    
    scheduler.schedule(task1);
    scheduler.schedule(task2);
    EXPECT_EQ(scheduler.getTaskCount(), 2);
    
    scheduler.runTasks(true);
    
    EXPECT_TRUE(task1Executed);
    EXPECT_TRUE(task2Executed);
    EXPECT_EQ(scheduler.getTaskCount(), 2); 
}

TEST_F(SchedulerImpl1Test, TaskExceptionHandling) {
    bool exceptionCaught = false;
    Task task([](){ throw std::runtime_error("Test exception"); }, __FILE__, __LINE__);
    task.addExceptionHandler([&exceptionCaught](const TaskExceptionContext& context) {
        exceptionCaught = true;
        EXPECT_TRUE(context.what().find("Test exception") != std::string::npos);
    });
    
    scheduler.schedule(task);
    EXPECT_EQ(scheduler.getTaskCount(), 1);
    
    scheduler.runTasks();
    
    EXPECT_TRUE(exceptionCaught);
    EXPECT_EQ(scheduler.getTaskCount(), 0);
}

TEST_F(SchedulerImpl1Test, GetTaskCount) {
    EXPECT_EQ(scheduler.getTaskCount(), 0);
    
    Task task1([]() {}, __FILE__, __LINE__);
    Task task2([]() {}, __FILE__, __LINE__);
    Task task3([]() {}, __FILE__, __LINE__);
    
    scheduler.schedule(task1);
    EXPECT_EQ(scheduler.getTaskCount(), 1);
    
    scheduler.schedule(task2);
    scheduler.schedule(task3);
    EXPECT_EQ(scheduler.getTaskCount(), 3);
    
    scheduler.unschedule(1);  // 假设任务ID从0开始
    EXPECT_EQ(scheduler.getTaskCount(), 2);
}

TEST_F(SchedulerImpl1Test, StopScheduler) {
    SchedulerImpl1 scheduler;
    int taskId = 0;
    taskId = scheduler.schedule(Task([](){ /* 简单任务 */ }, __FILE__, __LINE__));
    EXPECT_EQ(scheduler.getTaskCount(), 1);
    EXPECT_GE(taskId, 0);
    scheduler.stop();
    taskId = scheduler.schedule(Task([](){ /* 简单任务 */ }, __FILE__, __LINE__));
    EXPECT_LT(taskId, 0);
    EXPECT_EQ(scheduler.getTaskCount(), 1);
}

TEST_F(SchedulerImpl1Test, ConcurrentScheduling) {
    SchedulerImpl1 scheduler;
    const int numThreads = 10;
    const int tasksPerThread = 1000;
    std::vector<std::thread> threads;
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&scheduler, tasksPerThread]() {
            for (int j = 0; j < tasksPerThread; ++j) {
                scheduler.schedule(Task([](){ /* 简单任务 */ }, __FILE__, __LINE__));
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(scheduler.getTaskCount(), numThreads * tasksPerThread);
}

TEST_F(SchedulerImpl1Test, ConcurrentUnscheduling) {
    SchedulerImpl1 scheduler;
    const int numTasks = 1000;
    std::vector<int> taskIDs;
    
    for (int i = 0; i < numTasks; ++i) {
        taskIDs.push_back(scheduler.schedule(Task([](){ /* 简单任务 */ }, __FILE__, __LINE__)));
    }
    
    const int numThreads = 10;
    std::vector<std::thread> threads;
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&scheduler, &taskIDs, i, numThreads]() {
            for (int j = i; j < taskIDs.size(); j += numThreads) {
                scheduler.unschedule(taskIDs[j]);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(scheduler.getTaskCount(), 0);
}

TEST_F(SchedulerImpl1Test, ConcurrentTaskExecution) {
    SchedulerImpl1 scheduler;
    const int numTasks = 1000;
    std::atomic<int> executedTasks(0);
    std::atomic<bool> stopFlag(false);

    // 线程1: 执行任务
    std::thread runThread([&]() {
        while (!stopFlag.load()) {
            scheduler.runTasks();
            std::this_thread::yield();
        }
    });

    // 线程2: 调度任务
    std::thread scheduleThread([&]() {
        for (int i = 0; i < numTasks; ++i) {
            scheduler.schedule(Task([&executedTasks]() {
                executedTasks.fetch_add(1);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }, __FILE__, __LINE__));
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    scheduleThread.join();
    
    // 等待所有任务执行完毕
    while (executedTasks.load() < numTasks) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    stopFlag.store(true);
    runThread.join();

    EXPECT_EQ(executedTasks.load(), numTasks);
    EXPECT_EQ(scheduler.getTaskCount(), 0);
}


// int main(int argc, char **argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
