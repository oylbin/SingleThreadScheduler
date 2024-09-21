#include <gtest/gtest.h>
#include "SingleThreadScheduler/Scheduler.h"

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
    EXPECT_EQ(scheduler.getTaskCount(), 2);  // 任务执行后仍然保留在调度器中
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
    EXPECT_EQ(scheduler.getTaskCount(), 1);  // 异常任务仍然保留在调度器中
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

TEST_F(SchedulerImpl1Test, ClearTasks) {
    Task task1([]() {}, __FILE__, __LINE__);
    Task task2([]() {}, __FILE__, __LINE__);
    Task task3([]() {}, __FILE__, __LINE__);
    
    scheduler.schedule(task1);
    scheduler.schedule(task2);
    scheduler.schedule(task3);
    EXPECT_EQ(scheduler.getTaskCount(), 3);
    
    scheduler.clear();
    EXPECT_EQ(scheduler.getTaskCount(), 0);
    
    // 确保在清除后仍然可以添加新任务
    scheduler.schedule(task1);
    EXPECT_EQ(scheduler.getTaskCount(), 1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
