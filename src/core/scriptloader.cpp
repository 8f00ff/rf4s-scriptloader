#include "scriptloader.h"
#include "logger.h"
#include "hook_manager.h"
#include "config_manager.h"
#include <windows.h>
#include <filesystem>

namespace RF4S {

ScriptLoader& ScriptLoader::GetInstance() {
    static ScriptLoader instance;
    return instance;
}

ScriptLoader::~ScriptLoader() {
    Shutdown();
}

bool ScriptLoader::Initialize() {
    if (initialized) {
        return true;
    }
    
    // Get module base address
    moduleBase = reinterpret_cast<uintptr_t>(GetModuleHandleA(NULL));
    
    // Initialize core systems in order
    if (!InitializeLogging()) {
        return false;
    }
    
    RF4S_LOG_INFO("RF4S ScriptLoader v%s initializing...", version);
    RF4S_LOG_INFO("API Version: %s", apiVersion);
    RF4S_LOG_INFO("Module base: 0x%p", reinterpret_cast<void*>(moduleBase));
    
    if (!InitializeConfig()) {
        RF4S_LOG_ERROR("Failed to initialize configuration system");
        return false;
    }
    
    if (!InitializeHooks()) {
        RF4S_LOG_ERROR("Failed to initialize hook system");
        return false;
    }
    
    if (!InitializeMods()) {
        RF4S_LOG_ERROR("Failed to initialize mod system");
        return false;
    }
    
    initialized = true;
    RF4S_LOG_INFO("RF4S ScriptLoader initialization complete");
    return true;
}

void ScriptLoader::Shutdown() {
    if (!initialized) {
        return;
    }
    
    RF4S_LOG_INFO("RF4S ScriptLoader shutting down...");
    
    // Shutdown in reverse order
    HookManager::GetInstance().Shutdown();
    ConfigManager::GetInstance().Shutdown();
    
    RF4S_LOG_INFO("RF4S ScriptLoader shutdown complete");
    Logger::GetInstance().Shutdown();
    
    initialized = false;
}

bool ScriptLoader::InitializeLogging() {
    std::string logPath = GetLogPath();
    
    Logger& logger = Logger::GetInstance();
    if (!logger.Initialize(logPath)) {
        // Can't use RF4S_LOG_ERROR here since logger isn't initialized
        printf("Failed to initialize logger with path: %s\n", logPath.c_str());
        return false;
    }
    
    return true;
}

bool ScriptLoader::InitializeConfig() {
    std::string configPath = GetConfigPath();
    
    ConfigManager& config = ConfigManager::GetInstance();
    if (!config.Initialize(configPath)) {
        return false;
    }
    
    // Apply configuration settings
    Logger& logger = Logger::GetInstance();
    
    // Set log level from config
    std::string logLevelStr = config.GetString("core.log_level", "info");
    LogLevel logLevel = LogLevel::INFO;
    
    if (logLevelStr == "debug") {
        logLevel = LogLevel::DEBUG;
    } else if (logLevelStr == "info") {
        logLevel = LogLevel::INFO;
    } else if (logLevelStr == "warn") {
        logLevel = LogLevel::WARN;
    } else if (logLevelStr == "error") {
        logLevel = LogLevel::ERROR_LEVEL;
    } else {
        RF4S_LOG_WARN("Unknown log level '%s', using 'info'", logLevelStr.c_str());
    }
    
    logger.SetLogLevel(logLevel);
    
    // Set console enabled from config
    bool consoleEnabled = config.GetBool("core.enable_console", true);
    logger.SetConsoleEnabled(consoleEnabled);
    
    RF4S_LOG_INFO("Configuration loaded from: %s", configPath.c_str());
    return true;
}

bool ScriptLoader::InitializeHooks() {
    HookManager& hookManager = HookManager::GetInstance();
    return hookManager.Initialize();
}

bool ScriptLoader::InitializeMods() {
    // For now, we'll implement the "Everyone is Bi" mod directly
    // In the future, this will load mods from the mods directory
    
    RF4S_LOG_INFO("Mod system initialized (built-in mods only)");
    return true;
}

Logger& ScriptLoader::GetLogger() {
    return Logger::GetInstance();
}

HookManager& ScriptLoader::GetHookManager() {
    return HookManager::GetInstance();
}

ConfigManager& ScriptLoader::GetConfigManager() {
    return ConfigManager::GetInstance();
}

std::string ScriptLoader::GetExecutableDirectory() const {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    
    std::filesystem::path exePath(path);
    return exePath.parent_path().string();
}

std::string ScriptLoader::GetConfigPath() const {
    std::string exeDir = GetExecutableDirectory();
    return exeDir + "\\config\\scriptloader.toml";
}

std::string ScriptLoader::GetLogPath() const {
    std::string exeDir = GetExecutableDirectory();
    return exeDir + "\\rf4s_scriptloader.log";
}

} // namespace RF4S