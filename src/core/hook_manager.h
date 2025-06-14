#pragma once

#include <windows.h>
#include <string>
#include <unordered_map>
#include <memory>

namespace RF4S {

struct HookInfo {
    std::string name;
    void* target;
    void* detour;
    void* original;
    bool enabled;
};

class HookManager {
public:
    static HookManager& GetInstance();
    
    bool Initialize();
    void Shutdown();
    
    bool CreateHook(const std::string& name, void* target, void* detour, void** original);
    bool EnableHook(const std::string& name);
    bool DisableHook(const std::string& name);
    bool RemoveHook(const std::string& name);
    
    bool IsHookEnabled(const std::string& name) const;
    bool HookExists(const std::string& name) const;
    
    // Get hook statistics
    size_t GetHookCount() const;
    size_t GetEnabledHookCount() const;
    
private:
    HookManager() = default;
    ~HookManager();
    
    HookManager(const HookManager&) = delete;
    HookManager& operator=(const HookManager&) = delete;
    
    std::unordered_map<std::string, std::unique_ptr<HookInfo>> hooks;
    bool initialized = false;
    
    bool ValidateHookName(const std::string& name) const;
    bool ValidateTarget(void* target) const;
};

} // namespace RF4S