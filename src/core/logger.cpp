#include "logger.h"
#include <windows.h>
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace RF4S {

Logger& Logger::GetInstance() {
    static Logger instance;
    return instance;
}

Logger::~Logger() {
    Shutdown();
}

bool Logger::Initialize(const std::string& logPath) {
    if (initialized) {
        return true;
    }
    
    // Set up console output if enabled
    if (consoleEnabled) {
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
    }
    
    // Open log file
    logFile.open(logPath, std::ios::out | std::ios::trunc);
    if (!logFile.is_open()) {
        if (consoleEnabled) {
            printf("Failed to open log file at %s\n", logPath.c_str());
        }
        return false;
    }
    
    initialized = true;
    Log(LogLevel::INFO, "Logger initialized (log file: %s)", logPath.c_str());
    return true;
}

void Logger::Shutdown() {
    if (initialized) {
        Log(LogLevel::INFO, "Logger shutting down");
        if (logFile.is_open()) {
            logFile.close();
        }
        initialized = false;
    }
}

void Logger::Log(LogLevel level, const char* format, ...) {
    if (!initialized || level < currentLogLevel) {
        return;
    }
    
    // Get current time
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    
    // Format timestamp
    std::ostringstream timestamp;
    timestamp << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    
    // Format the message
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    // Create full log message
    std::string fullMessage = "[" + timestamp.str() + "] [" + GetLogLevelString(level) + "] " + buffer + "\n";
    
    // Write to console if enabled
    if (consoleEnabled) {
        printf("%s", fullMessage.c_str());
    }
    
    // Write to log file
    if (logFile.is_open()) {
        logFile << fullMessage << std::flush;
    }
}

void Logger::SetLogLevel(LogLevel level) {
    currentLogLevel = level;
}

void Logger::SetConsoleEnabled(bool enabled) {
    consoleEnabled = enabled;
}

const char* Logger::GetLogLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR_LEVEL: return "ERROR";
        default: return "UNKNOWN";
    }
}

} // namespace RF4S