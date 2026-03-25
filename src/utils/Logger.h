#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <memory>

namespace Minecraft {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

struct LogMessage {
    LogLevel level;
    std::string message;
    std::string timestamp;
    std::thread::id threadId;
};

class Logger {
public:
    static void Init(const std::string& logFile = "minecraft.log", bool consoleOutput = true, bool asyncMode = true);
    static void Shutdown();
    
    static void Debug(const std::string& message);
    static void Info(const std::string& message);
    static void Warning(const std::string& message);
    static void Error(const std::string& message);
    static void Fatal(const std::string& message);
    
    static void SetLogLevel(LogLevel level) { s_MinLogLevel = level; }
    static void EnableFileOutput(bool enable) { s_FileOutputEnabled = enable; }
    static void EnableConsoleOutput(bool enable) { s_ConsoleOutputEnabled = enable; }
    static void Flush(); // 强制刷新所有待处理日志

private:
    Logger() = default;
    ~Logger() = default;
    
    static void Log(LogLevel level, const std::string& message);
    static void ProcessLogQueue(); // 异步日志处理线程
    static void WriteLog(const LogMessage& logMsg);
    static std::string GetTimestamp();
    static std::string GetLevelString(LogLevel level);
    static std::string GetLevelColor(LogLevel level);
    static std::string GetThreadIdString(std::thread::id threadId);
    
    static std::ofstream s_LogFile;
    static std::mutex s_Mutex;
    static std::mutex s_QueueMutex;
    static std::condition_variable s_QueueCV;
    static std::queue<LogMessage> s_LogQueue;
    static std::thread* s_LogThread; // 改用原始指针
    static std::atomic<bool> s_Running;
    static std::atomic<bool> s_AsyncMode;
    static LogLevel s_MinLogLevel;
    static bool s_FileOutputEnabled;
    static bool s_ConsoleOutputEnabled;
};

// Convenience macros
#define LOG_DEBUG(msg)   Minecraft::Logger::Debug(msg)
#define LOG_INFO(msg)    Minecraft::Logger::Info(msg)
#define LOG_WARNING(msg) Minecraft::Logger::Warning(msg)
#define LOG_ERROR(msg)   Minecraft::Logger::Error(msg)
#define LOG_FATAL(msg)   Minecraft::Logger::Fatal(msg)

// Format helper
template<typename... Args>
std::string Format(const std::string& format, Args... args) {
    size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1);
}

} // namespace Minecraft
