#pragma once

#include <fstream>
#include <string>

namespace RF4S {

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR_LEVEL = 3
};

class Logger {
public:
    static Logger& GetInstance();
    
    bool Initialize(const std::string& logPath);
    void Shutdown();
    
    void Log(LogLevel level, const char* format, ...);
    void SetLogLevel(LogLevel level);
    void SetConsoleEnabled(bool enabled);
    
private:
    Logger() = default;
    ~Logger();
    
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    std::ofstream logFile;
    LogLevel currentLogLevel = LogLevel::INFO;
    bool consoleEnabled = true;
    bool initialized = false;
    
    const char* GetLogLevelString(LogLevel level);
};

// Convenience macros
#define RF4S_LOG_DEBUG(fmt, ...) RF4S::Logger::GetInstance().Log(RF4S::LogLevel::DEBUG, fmt, ##__VA_ARGS__)
#define RF4S_LOG_INFO(fmt, ...)  RF4S::Logger::GetInstance().Log(RF4S::LogLevel::INFO, fmt, ##__VA_ARGS__)
#define RF4S_LOG_WARN(fmt, ...)  RF4S::Logger::GetInstance().Log(RF4S::LogLevel::WARN, fmt, ##__VA_ARGS__)
#define RF4S_LOG_ERROR(fmt, ...) RF4S::Logger::GetInstance().Log(RF4S::LogLevel::ERROR_LEVEL, fmt, ##__VA_ARGS__)

} // namespace RF4S