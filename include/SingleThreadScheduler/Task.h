#pragma once

#include <iostream>
#include <functional>
#include <string>
#include <list>
#include <chrono>

class TaskExceptionContext {
public:
    TaskExceptionContext(const std::string& what, const std::string& file, int line) : what_(what), file_(file), line_(line) {}
    std::string file_;
    int line_;
    std::string what_;
    std::string what() const {
        return "task created at " + file_ + ":" + std::to_string(line_) + " - " + what_;
    }
};

class Task {
public:
    // 添加默认构造函数
    // 必须有默认构造函数，才能放入 std::unordered_map
    Task() : func_([]{}), file_(""), line_(0), created_at_(std::chrono::steady_clock::now()), 
             start_time_(std::chrono::steady_clock::time_point()), 
             end_time_(std::chrono::steady_clock::time_point()) {}

    Task(std::function<void()> func, const std::string& file, int line)
        : func_(func), file_(file), line_(line), created_at_(std::chrono::steady_clock::now()),
          start_time_(std::chrono::steady_clock::time_point()), 
          end_time_(std::chrono::steady_clock::time_point()) {}

    void execute() {
        start_time_ = std::chrono::steady_clock::now();
        try {
            func_();
        } catch (const std::exception& e) {
            std::cerr << "Exception in task scheduled at " << file_ << ":" << line_
                      << " - " << e.what() << std::endl;
            // 你可以在这里进一步处理异常，如记录日志或重新抛出
            auto context = TaskExceptionContext(e.what(), file_, line_);
            for (auto& handler : exceptionHandlers_) {
                handler(context);
            }
        } catch (...) {
            std::cerr << "Unknown exception in task scheduled at " << file_ << ":" << line_ << std::endl;
            // 处理未知异常
            auto context = TaskExceptionContext("Unknown exception", file_, line_);
            for (auto& handler : exceptionHandlers_) {
                handler(context);
            }
        }
        end_time_ = std::chrono::steady_clock::now();

        // 计算执行时间
//        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_ - start_time_);
//        std::cout << "任务执行时间: " << duration.count() << " 毫秒" << std::endl;
    }
    void addExceptionHandler(std::function<void(const TaskExceptionContext&)> func) {
        exceptionHandlers_.push_back(func);
    }

    std::chrono::steady_clock::time_point getCreatedAt() const { return created_at_; }
    
    bool hasStarted() const { 
        return start_time_ != std::chrono::steady_clock::time_point(); 
    }
    
    bool hasEnded() const { 
        return end_time_ != std::chrono::steady_clock::time_point(); 
    }
    
    std::chrono::steady_clock::time_point getStartTime() const {
        return start_time_;
    }
    
    std::chrono::steady_clock::time_point getEndTime() const {
        return end_time_;
    }

    std::chrono::steady_clock::duration getDuration() const {
        return end_time_ - start_time_;
    }

    std::chrono::steady_clock::duration getWaitingTime() const {
        return start_time_ - created_at_;
    }


private:
    std::function<void()> func_;
    std::list<std::function<void(const TaskExceptionContext&)>> exceptionHandlers_;
    std::string file_;
    int line_;
    std::chrono::steady_clock::time_point created_at_;
    std::chrono::steady_clock::time_point start_time_;
    std::chrono::steady_clock::time_point end_time_;
};