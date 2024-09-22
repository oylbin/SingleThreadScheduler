cmake_minimum_required(VERSION 3.14)

project(singlethreadscheduler VERSION 1.0)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# 添加一个选项来控制是否构建测试
option(BUILD_TESTING "Build the testing tree." OFF)

# 设置头文件目录
include_directories(include)

# 收集源文件
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 创建库
add_library(singlethreadscheduler ${SOURCES})

# 设置头文件目录
target_include_directories(singlethreadscheduler PUBLIC include)

# 只有当 BUILD_TESTING 为 ON 时才构建测试
if(BUILD_TESTING)
    # 启用测试
    enable_testing()

    # 使用 Conan 包
    find_package(GTest REQUIRED CONFIG)

    # 添加测试
    file(GLOB_RECURSE TEST_SOURCES "test/*.cpp")
    if(TEST_SOURCES)
        add_executable(singlethreadschedulertest ${TEST_SOURCES})
        target_link_libraries(singlethreadschedulertest
          PRIVATE
          singlethreadscheduler
            GTest::gtest
            GTest::gtest_main
        )

        # 添加测试到CTest
        include(GoogleTest)
        gtest_discover_tests(singlethreadschedulertest)
    else()
        message(WARNING "No test sources found. Tests will not be built.")
    endif()
    if(WIN32)
        target_compile_options(singlethreadschedulertest PUBLIC "$<$<C_COMPILER_ID:MSVC>:/utf-8>")
        target_compile_options(singlethreadschedulertest PUBLIC "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    endif()
endif()

# 添加安装
install(TARGETS singlethreadscheduler
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

install(DIRECTORY include/
  DESTINATION include
)
if(WIN32)
    target_compile_options(singlethreadscheduler PUBLIC "$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    target_compile_options(singlethreadscheduler PUBLIC "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()
