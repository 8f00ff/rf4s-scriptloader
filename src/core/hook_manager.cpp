#include "hook_manager.h"
#include "logger.h"
#include "../../extern/minhook/include/MinHook.h"

namespace RF4S {

HookManager& HookManager::GetInstance() {
    static HookManager instance;
    return instance;
}

HookManager::~HookManager() {
    Shutdown();
}

bool HookManager::Initialize() {
    if (initialized) {
        RF4S_LOG_WARN("HookManager already initialized");
        return true;
    }
    
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK) {
        RF4S_LOG_ERROR("Failed to initialize MinHook: %d", status);
        return false;
    }
    
    initialized = true;
    RF4S_LOG_INFO("HookManager initialized successfully");
    return true;
}

void HookManager::Shutdown() {
    if (!initialized) {
        return;
    }
    
    RF4S_LOG_INFO("Shutting down HookManager");
    
    // Disable and remove all hooks
    for (auto& [name, hook] : hooks) {
        if (hook->enabled) {
            MH_DisableHook(hook->target);
        }
        MH_RemoveHook(hook->target);
    }
    
    hooks.clear();
    MH_Uninitialize();
    initialized = false;
    
    RF4S_LOG_INFO("HookManager shutdown complete");
}

bool HookManager::CreateHook(const std::string& name, void* target, void* detour, void** original) {
    if (!initialized) {
        RF4S_LOG_ERROR("HookManager not initialized");
        return false;
    }
    
    if (!ValidateHookName(name)) {
        RF4S_LOG_ERROR("Invalid hook name: %s", name.c_str());
        return false;
    }
    
    if (!ValidateTarget(target)) {
        RF4S_LOG_ERROR("Invalid target address for hook: %s", name.c_str());
        return false;
    }
    
    if (HookExists(name)) {
        RF4S_LOG_ERROR("Hook already exists: %s", name.c_str());
        return false;
    }
    
    RF4S_LOG_INFO("Creating hook '%s' at %p", name.c_str(), target);
    
    void* originalFunc = nullptr;
    MH_STATUS status = MH_CreateHook(target, detour, &originalFunc);
    
    if (status != MH_OK) {
        RF4S_LOG_ERROR("Failed to create hook '%s': %d", name.c_str(), status);
        return false;
    }
    
    // Store hook information
    auto hookInfo = std::make_unique<HookInfo>();
    hookInfo->name = name;
    hookInfo->target = target;
    hookInfo->detour = detour;
    hookInfo->original = originalFunc;
    hookInfo->enabled = false;
    
    hooks[name] = std::move(hookInfo);
    
    if (original) {
        *original = originalFunc;
    }
    
    RF4S_LOG_INFO("Hook '%s' created successfully", name.c_str());
    return true;
}

bool HookManager::EnableHook(const std::string& name) {
    if (!initialized) {
        RF4S_LOG_ERROR("HookManager not initialized");
        return false;
    }
    
    auto it = hooks.find(name);
    if (it == hooks.end()) {
        RF4S_LOG_ERROR("Hook not found: %s", name.c_str());
        return false;
    }
    
    if (it->second->enabled) {
        RF4S_LOG_WARN("Hook '%s' is already enabled", name.c_str());
        return true;
    }
    
    MH_STATUS status = MH_EnableHook(it->second->target);
    if (status != MH_OK) {
        RF4S_LOG_ERROR("Failed to enable hook '%s': %d", name.c_str(), status);
        return false;
    }
    
    it->second->enabled = true;
    RF4S_LOG_INFO("Hook '%s' enabled successfully", name.c_str());
    return true;
}

bool HookManager::DisableHook(const std::string& name) {
    if (!initialized) {
        RF4S_LOG_ERROR("HookManager not initialized");
        return false;
    }
    
    auto it = hooks.find(name);
    if (it == hooks.end()) {
        RF4S_LOG_ERROR("Hook not found: %s", name.c_str());
        return false;
    }
    
    if (!it->second->enabled) {
        RF4S_LOG_WARN("Hook '%s' is already disabled", name.c_str());
        return true;
    }
    
    MH_STATUS status = MH_DisableHook(it->second->target);
    if (status != MH_OK) {
        RF4S_LOG_ERROR("Failed to disable hook '%s': %d", name.c_str(), status);
        return false;
    }
    
    it->second->enabled = false;
    RF4S_LOG_INFO("Hook '%s' disabled successfully", name.c_str());
    return true;
}

bool HookManager::RemoveHook(const std::string& name) {
    if (!initialized) {
        RF4S_LOG_ERROR("HookManager not initialized");
        return false;
    }
    
    auto it = hooks.find(name);
    if (it == hooks.end()) {
        RF4S_LOG_ERROR("Hook not found: %s", name.c_str());
        return false;
    }
    
    // Disable hook first if it's enabled
    if (it->second->enabled) {
        MH_DisableHook(it->second->target);
    }
    
    MH_STATUS status = MH_RemoveHook(it->second->target);
    if (status != MH_OK) {
        RF4S_LOG_ERROR("Failed to remove hook '%s': %d", name.c_str(), status);
        return false;
    }
    
    hooks.erase(it);
    RF4S_LOG_INFO("Hook '%s' removed successfully", name.c_str());
    return true;
}

bool HookManager::IsHookEnabled(const std::string& name) const {
    auto it = hooks.find(name);
    return it != hooks.end() && it->second->enabled;
}

bool HookManager::HookExists(const std::string& name) const {
    return hooks.find(name) != hooks.end();
}

size_t HookManager::GetHookCount() const {
    return hooks.size();
}

size_t HookManager::GetEnabledHookCount() const {
    size_t count = 0;
    for (const auto& [name, hook] : hooks) {
        if (hook->enabled) {
            count++;
        }
    }
    return count;
}

bool HookManager::ValidateHookName(const std::string& name) const {
    return !name.empty() && name.length() <= 256;
}

bool HookManager::ValidateTarget(void* target) const {
    if (!target) {
        return false;
    }
    
    // Basic validation - check if the address is in a reasonable range
    uintptr_t addr = reinterpret_cast<uintptr_t>(target);
    return addr > 0x10000; // Avoid null pointer and very low addresses
}

} // namespace RF4S