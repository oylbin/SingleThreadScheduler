#include <gtest/gtest.h>
#include "SingleThreadScheduler/Task.h"
#include <thread>
#include <chrono>
using namespace singlethreadscheduler;
// 测试正常执行的任务
TEST(TaskTest, NormalExecution) {
    bool executed = false;
    Task task([&executed]() { executed = true; }, __FILE__, __LINE__);
    task.execute();
    EXPECT_TRUE(executed);
}

// 测试抛出std::exception的任务
TEST(TaskTest, ExceptionHandling) {
    Task task([]() { throw std::runtime_error("Test exception"); }, __FILE__, __LINE__);
    
    bool exceptionHandled = false;
    task.addExceptionHandler([&exceptionHandled](const TaskExceptionContext& context) {
        exceptionHandled = true;
        EXPECT_STREQ(context.what_.c_str(), "Test exception");
    });

    testing::internal::CaptureStderr();
    task.execute();
    std::string output = testing::internal::GetCapturedStderr();

    EXPECT_TRUE(exceptionHandled);
    EXPECT_TRUE(output.find("Exception in task scheduled at") != std::string::npos);
    EXPECT_TRUE(output.find("Test exception") != std::string::npos);
}

// 测试抛出未知异常的任务
TEST(TaskTest, UnknownExceptionHandling) {
    Task task([]() { throw 42; }, __FILE__, __LINE__);
    
    bool exceptionHandled = false;
    task.addExceptionHandler([&exceptionHandled](const TaskExceptionContext& context) {
        exceptionHandled = true;
        std::cout << "context.what_: " << context.what_ << std::endl;
        EXPECT_STREQ(context.what_.c_str(), "Unknown exception");
    });

    testing::internal::CaptureStderr();
    task.execute();
    std::string output = testing::internal::GetCapturedStderr();

    EXPECT_TRUE(exceptionHandled);
    EXPECT_TRUE(output.find("Unknown exception in task scheduled at") != std::string::npos);
}

// 测试多个异常处理器
TEST(TaskTest, MultipleExceptionHandlers) {
    Task task([]() { throw std::runtime_error("Multiple handlers test"); }, __FILE__, __LINE__);
    
    int handlersCalled = 0;
    task.addExceptionHandler([&handlersCalled](const TaskExceptionContext&) { handlersCalled++; });
    task.addExceptionHandler([&handlersCalled](const TaskExceptionContext&) { handlersCalled++; });

    task.execute();

    EXPECT_EQ(handlersCalled, 2);
}

// 测试任务执行和计时功能
TEST(TaskTest, ExecutionAndTiming) {
    std::string output;
    Task task([&output]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        output = "Task executed";
    }, __FILE__, __LINE__);

    EXPECT_FALSE(task.hasStarted());
    EXPECT_FALSE(task.hasEnded());

    task.execute();

    EXPECT_TRUE(task.hasStarted());
    EXPECT_TRUE(task.hasEnded());
    EXPECT_EQ(output, "Task executed");

    auto duration = task.getDuration();
    EXPECT_GE(duration, std::chrono::milliseconds(50));
    EXPECT_LT(duration, std::chrono::milliseconds(100));  // 允许一些误差

    auto waitingTime = task.getWaitingTime();
    EXPECT_LT(waitingTime, std::chrono::milliseconds(10));  // 假设创建后很快就执行了
}
