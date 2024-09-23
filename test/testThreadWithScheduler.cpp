#include <gtest/gtest.h>
#include "SingleThreadScheduler/ThreadWithScheduler.h"
#include <chrono>
#include <thread>
using namespace singlethreadscheduler;
class ThreadWithSchedulerTest : public ::testing::Test {
protected:
    void SetUp() override {
        thread = new ThreadWithScheduler();
    }

    void TearDown() override {
        delete thread;
    }

    ThreadWithScheduler *thread;
};

TEST_F(ThreadWithSchedulerTest, InitTest) {
    EXPECT_NE(thread->getScheduler(), nullptr);
}

TEST_F(ThreadWithSchedulerTest, StartAndStopTest) {
    thread->start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    thread->stopAndJoin();
}

TEST_F(ThreadWithSchedulerTest, ScheduleTaskTest) {
    std::atomic<int> counter(0);
    auto task = [&counter]() { counter++; };

    thread->start();
    thread->getScheduler()->scheduleOnce(Task(task, __FILE__, __LINE__));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    thread->stopAndJoin();

    EXPECT_EQ(counter, 1);
}


TEST_F(ThreadWithSchedulerTest, SetLoopTimeTest) {
    std::atomic<int> counter(0);
    auto task = [&counter]() {
        auto now = std::chrono::steady_clock::now();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        std::cout << "milliseconds: " << milliseconds << std::endl;
        counter++;
        };

    thread->setLoopTimeMS(100);
    thread->start();
    thread->getScheduler()->scheduleRepeat(Task(task, __FILE__, __LINE__));

    std::this_thread::sleep_for(std::chrono::milliseconds(230));
    thread->stopAndJoin();

    // 预期执行次数应该在2到4次之间（考虑到一些误差）
    EXPECT_GE(counter, 2);
    EXPECT_LE(counter, 4);
}

// int main(int argc, char **argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
