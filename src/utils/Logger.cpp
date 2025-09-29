#include "Logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace Minecraft {

std::ofstream Logger::s_logFile;
Logger::Level Logger::s_currentLevel = Logger::Level::Info;
std::mutex Logger::s_mutex;
bool Logger::s_initialized = false;

void Logger::Initialize(const std::string& logFile) {
    std::lock_guard<std::mutex> lock(s_mutex);
    
    if (s_initialized) {
        return;
    }

    if (!logFile.empty()) {
        s_logFile.open(logFile, std::ios::out | std::ios::app);
        if (!s_logFile.is_open()) {
            std::cerr << "Warning: Failed to open log file: " << logFile << std::endl;
        }
    }

    s_initialized = true;
    
    // 在锁外记录初始化信息
    std::string timeStr = GetCurrentTimeString();
    std::string logMessage = "[" + timeStr + "] [INFO ] Logger initialized";
    std::cout << logMessage << std::endl;
    
    if (s_logFile.is_open()) {
        s_logFile << logMessage << std::endl;
        s_logFile.flush();
    }
}

void Logger::Shutdown() {
    std::lock_guard<std::mutex> lock(s_mutex);
    
    if (s_logFile.is_open()) {
        s_logFile.close();
    }
    
    s_initialized = false;
}

void Logger::Debug(const std::string& message) {
    Log(Level::Debug, message);
}

void Logger::Info(const std::string& message) {
    Log(Level::Info, message);
}

void Logger::Warning(const std::string& message) {
    Log(Level::Warning, message);
}

void Logger::Error(const std::string& message) {
    Log(Level::Error, message);
}

void Logger::SetLevel(Level level) {
    std::lock_guard<std::mutex> lock(s_mutex);
    s_currentLevel = level;
}

void Logger::Log(Level level, const std::string& message) {
    std::lock_guard<std::mutex> lock(s_mutex);
    
    if (level < s_currentLevel) {
        return;
    }

    std::string timeStr = GetCurrentTimeString();
    std::string levelStr = LevelToString(level);
    std::string logMessage = "[" + timeStr + "] [" + levelStr + "] " + message;

    // 输出到控制台
    if (level == Level::Error) {
        std::cerr << logMessage << std::endl;
    } else {
        std::cout << logMessage << std::endl;
    }

    // 输出到文件
    if (s_logFile.is_open()) {
        s_logFile << logMessage << std::endl;
        s_logFile.flush();
    }
}

std::string Logger::GetCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

std::string Logger::LevelToString(Level level) {
    switch (level) {
        case Level::Debug:   return "DEBUG";
        case Level::Info:    return "INFO ";
        case Level::Warning: return "WARN ";
        case Level::Error:   return "ERROR";
        default:             return "UNKNOWN";
    }
}

} // namespace Minecraft
