#pragma once

#include <string>
#include <mutex>
#include <iostream>

class Logger {
public:
    static Logger& getInstance();
    void log(const std::string &msg);
private:
    Logger() = default;
    std::mutex mtx;
};
