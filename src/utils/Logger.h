#pragma once

#include <string>
#include <fstream>
#include <mutex>

namespace Minecraft {

/**
 * 日志记录器
 * 提供统一的日志输出接口
 */
class Logger {
public:
    enum class Level {
        Debug,
        Info,
        Warning,
        Error
    };

    /**
     * 初始化日志系统
     * @param logFile 日志文件路径，空字符串表示不输出到文件
     */
    static void Initialize(const std::string& logFile = "");

    /**
     * 清理日志系统
     */
    static void Shutdown();

    /**
     * 记录调试信息
     */
    static void Debug(const std::string& message);

    /**
     * 记录一般信息
     */
    static void Info(const std::string& message);

    /**
     * 记录警告信息
     */
    static void Warning(const std::string& message);

    /**
     * 记录错误信息
     */
    static void Error(const std::string& message);

    /**
     * 设置日志级别
     */
    static void SetLevel(Level level);

private:
    /**
     * 内部日志记录方法
     */
    static void Log(Level level, const std::string& message);

    /**
     * 获取当前时间字符串
     */
    static std::string GetCurrentTimeString();

    /**
     * 级别转换为字符串
     */
    static std::string LevelToString(Level level);

private:
    static std::ofstream s_logFile;
    static Level s_currentLevel;
    static std::mutex s_mutex;
    static bool s_initialized;
};
} // namespace Minecraft
