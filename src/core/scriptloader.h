#pragma once

#include <string>
#include <memory>

namespace RF4S {

class ScriptLoader {
public:
    static ScriptLoader& GetInstance();
    
    bool Initialize();
    void Shutdown();
    
    // Core initialization steps
    bool InitializeLogging();
    bool InitializeConfig();
    bool InitializeHooks();
    bool InitializeMods();
    
    // Getters for core systems
    class Logger& GetLogger();
    class HookManager& GetHookManager();
    class ConfigManager& GetConfigManager();
    
    // Game information
    uintptr_t GetModuleBase() const { return moduleBase; }
    const char* GetVersion() const { return version; }
    const char* GetAPIVersion() const { return apiVersion; }
    
    // Status
    bool IsInitialized() const { return initialized; }
    
private:
    ScriptLoader() = default;
    ~ScriptLoader();
    
    ScriptLoader(const ScriptLoader&) = delete;
    ScriptLoader& operator=(const ScriptLoader&) = delete;
    
    bool initialized = false;
    uintptr_t moduleBase = 0;
    
    // Version information
    static constexpr const char* version = "2.0.0";
    static constexpr const char* apiVersion = "1.0.0";
    
    // Helper methods
    std::string GetExecutableDirectory() const;
    std::string GetConfigPath() const;
    std::string GetLogPath() const;
};

} // namespace RF4S