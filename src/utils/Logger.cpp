#include "Logger.h"

namespace Minecraft {

std::ofstream Logger::s_LogFile;
std::mutex Logger::s_Mutex;
std::mutex Logger::s_QueueMutex;
std::condition_variable Logger::s_QueueCV;
std::queue<LogMessage> Logger::s_LogQueue;
std::thread* Logger::s_LogThread = nullptr;
std::atomic<bool> Logger::s_Running(false);
std::atomic<bool> Logger::s_AsyncMode(true);
LogLevel Logger::s_MinLogLevel = LogLevel::Debug;
bool Logger::s_FileOutputEnabled = true;
bool Logger::s_ConsoleOutputEnabled = true;

void Logger::Init(const std::string& logFile, bool consoleOutput, bool asyncMode) {
    std::lock_guard<std::mutex> lock(s_Mutex);
    
    s_ConsoleOutputEnabled = consoleOutput;
    s_AsyncMode = asyncMode;
    
    if (s_FileOutputEnabled) {
        s_LogFile.open(logFile, std::ios::out | std::ios::app);
        if (s_LogFile.is_open()) {
            s_LogFile << "\n========== Minecraft Started at " << GetTimestamp() << " ==========\n";
            s_LogFile.flush();
        }
    }
    
    // 启动异步日志线程
    if (s_AsyncMode) {
        s_Running = true;
        s_LogThread = new std::thread(ProcessLogQueue);
    }
}

void Logger::Shutdown() {
    // 停止异步线程
    if (s_AsyncMode && s_LogThread) {
        s_Running = false;
        s_QueueCV.notify_all();
        if (s_LogThread->joinable()) {
            s_LogThread->join();
        }
        delete s_LogThread;
        s_LogThread = nullptr;
    }
    
    // 处理剩余的日志
    Flush();
    
    std::lock_guard<std::mutex> lock(s_Mutex);
    
    if (s_LogFile.is_open()) {
        s_LogFile << "========== Minecraft Shutdown at " << GetTimestamp() << " ==========\n\n";
        s_LogFile.close();
    }
}

void Logger::Debug(const std::string& message) {
    Log(LogLevel::Debug, message);
}

void Logger::Info(const std::string& message) {
    Log(LogLevel::Info, message);
}

void Logger::Warning(const std::string& message) {
    Log(LogLevel::Warning, message);
}

void Logger::Error(const std::string& message) {
    Log(LogLevel::Error, message);
}

void Logger::Fatal(const std::string& message) {
    Log(LogLevel::Fatal, message);
    // Fatal 级别立即刷新
    Flush();
}

void Logger::Log(LogLevel level, const std::string& message) {
    if (level < s_MinLogLevel) return;
    
    LogMessage logMsg;
    logMsg.level = level;
    logMsg.message = message;
    logMsg.timestamp = GetTimestamp();
    logMsg.threadId = std::this_thread::get_id();
    
    if (s_AsyncMode) {
        // 异步模式：加入队列
        {
            std::lock_guard<std::mutex> lock(s_QueueMutex);
            s_LogQueue.push(logMsg);
        }
        s_QueueCV.notify_one();
    } else {
        // 同步模式：直接写入
        WriteLog(logMsg);
    }
}

void Logger::ProcessLogQueue() {
    while (s_Running) {
        std::unique_lock<std::mutex> lock(s_QueueMutex);
        
        // 等待新日志或退出信号
        s_QueueCV.wait(lock, [] {
            return !s_LogQueue.empty() || !s_Running;
        });
        
        // 批量处理队列中的日志
        while (!s_LogQueue.empty()) {
            LogMessage logMsg = s_LogQueue.front();
            s_LogQueue.pop();
            lock.unlock();
            
            WriteLog(logMsg);
            
            lock.lock();
        }
    }
}

void Logger::WriteLog(const LogMessage& logMsg) {
    std::lock_guard<std::mutex> lock(s_Mutex);
    
    std::string levelStr = GetLevelString(logMsg.level);
    std::string threadStr = GetThreadIdString(logMsg.threadId);
    std::string logMessage = "[" + logMsg.timestamp + "] [" + threadStr + "] [" + levelStr + "] " + logMsg.message;
    
    // Console output with color
    if (s_ConsoleOutputEnabled) {
        std::string color = GetLevelColor(logMsg.level);
        std::cout << color << logMessage << "\033[0m" << std::endl;
    }
    
    // File output
    if (s_FileOutputEnabled && s_LogFile.is_open()) {
        s_LogFile << logMessage << std::endl;
        // 对于 Error 和 Fatal 级别立即刷新
        if (logMsg.level >= LogLevel::Error) {
            s_LogFile.flush();
        }
    }
}

void Logger::Flush() {
    if (s_AsyncMode) {
        // 等待队列清空
        while (true) {
            std::lock_guard<std::mutex> lock(s_QueueMutex);
            if (s_LogQueue.empty()) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    std::lock_guard<std::mutex> lock(s_Mutex);
    if (s_LogFile.is_open()) {
        s_LogFile.flush();
    }
}

std::string Logger::GetTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string Logger::GetLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO ";
        case LogLevel::Warning: return "WARN ";
        case LogLevel::Error:   return "ERROR";
        case LogLevel::Fatal:   return "FATAL";
        default:                return "UNKNOWN";
    }
}

std::string Logger::GetLevelColor(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:   return "\033[36m"; // Cyan
        case LogLevel::Info:    return "\033[32m"; // Green
        case LogLevel::Warning: return "\033[33m"; // Yellow
        case LogLevel::Error:   return "\033[31m"; // Red
        case LogLevel::Fatal:   return "\033[35m"; // Magenta
        default:                return "\033[0m";  // Reset
    }
}

std::string Logger::GetThreadIdString(std::thread::id threadId) {
    std::stringstream ss;
    ss << "T-" << std::hex << std::setw(4) << std::setfill('0') 
       << (std::hash<std::thread::id>{}(threadId) % 0xFFFF);
    return ss.str();
}

} // namespace Minecraft
