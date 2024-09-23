#include <gtest/gtest.h>
#include "SingleThreadScheduler/Scheduler.h"
#include <chrono>
#include <thread>
using namespace singlethreadscheduler;
class SingleThreadSchedulerImpl1Test : public ::testing::Test {
protected:
    SingleThreadSchedulerImpl1 *scheduler;

    void SetUp() override {
        scheduler = new SingleThreadSchedulerImpl1(std::this_thread::get_id());
    }

    void TearDown() override {
        delete scheduler;
    }
};

TEST_F(SingleThreadSchedulerImpl1Test, ScheduleOnce) {
    int count = 0;
    Task task([&count]() { count++; }, __FILE__, __LINE__);

    int taskId = scheduler->scheduleOnce(task);
    EXPECT_GE(taskId, 0);

    scheduler->update();
    EXPECT_EQ(count, 1);

    scheduler->update();
    EXPECT_EQ(count, 1);
}

TEST_F(SingleThreadSchedulerImpl1Test, ScheduleRepeat) {
    int count = 0;
    Task task([&count]() { count++; }, __FILE__, __LINE__);

    int taskId = scheduler->scheduleRepeat(task);
    EXPECT_GE(taskId, 0);

    scheduler->update();
    EXPECT_EQ(count, 1);

    scheduler->update();
    EXPECT_EQ(count, 2);
}

TEST_F(SingleThreadSchedulerImpl1Test, UnscheduleRepeat) {
    int count = 0;
    Task task([&count]() { count++; }, __FILE__, __LINE__);

    int taskId = scheduler->scheduleRepeat(task);
    EXPECT_GE(taskId, 0);

    scheduler->update();
    EXPECT_EQ(count, 1);

    bool unscheduled = scheduler->unscheduleRepeat(taskId);
    EXPECT_TRUE(unscheduled);

    scheduler->update();
    EXPECT_EQ(count, 1);
}


TEST_F(SingleThreadSchedulerImpl1Test, Stop) {
    int count = 0;
    Task task([&count]() { count++; }, __FILE__, __LINE__);

    scheduler->scheduleRepeat(task);
    scheduler->update();
    EXPECT_EQ(count, 1);

    scheduler->stop();
    scheduler->update();
    EXPECT_EQ(count, 1);
}

// int main(int argc, char **argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
