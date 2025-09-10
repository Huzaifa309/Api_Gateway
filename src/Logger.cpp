#include "Logger.h"

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(const std::string &msg) {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << msg << std::endl;
}