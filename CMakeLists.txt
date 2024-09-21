cmake_minimum_required(VERSION 3.14)

project(ThreadBondScheduler VERSION 1.0)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用测试
enable_testing()

# 添加Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)
FetchContent_MakeAvailable(googletest)

# 设置头文件目录
include_directories(include)

# 收集源文件
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 创建库
add_library(ThreadBondScheduler ${SOURCES})

# 设置头文件目录
target_include_directories(ThreadBondScheduler PUBLIC include)

# 添加测试
file(GLOB_RECURSE TEST_SOURCES "test/*.cpp")
add_executable(ThreadBondSchedulerTests ${TEST_SOURCES})
target_link_libraries(ThreadBondSchedulerTests
  PRIVATE
    ThreadBondScheduler
    gtest_main
)

# 添加测试到CTest
include(GoogleTest)
gtest_discover_tests(ThreadBondSchedulerTests)
