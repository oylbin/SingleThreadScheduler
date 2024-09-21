#pragma once

#include <iostream>
#include <functional>
#include <string>
#include <list>

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
    Task() : func_([]{}), file_(""), line_(0) {}

    Task(std::function<void()> func, const std::string& file, int line)
        : func_(func), file_(file), line_(line) {}

    void execute() const {
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
    }
    void addExceptionHandler(std::function<void(const TaskExceptionContext&)> func) {
        exceptionHandlers_.push_back(func);
    }

private:
    std::function<void()> func_;
    std::list<std::function<void(const TaskExceptionContext&)>> exceptionHandlers_;
    std::string file_;
    int line_;
};