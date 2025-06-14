#include "rf4s_api.h"
#include "../core/scriptloader.h"
#include "../core/logger.h"
#include "../core/hook_manager.h"
#include "../core/config_manager.h"
#include <cstdarg>
#include <unordered_map>
#include <string>

// Static storage for string returns (to ensure C API compatibility)
static std::unordered_map<std::string, std::string> stringCache;

// Helper function to cache strings for C API
static const char* CacheString(const std::string& str) {
    static size_t counter = 0;
    std::string key = "cached_" + std::to_string(counter++);
    stringCache[key] = str;
    return stringCache[key].c_str();
}

// Helper function to convert RF4S log levels to internal log levels
static RF4S::LogLevel ConvertLogLevel(RF4SLogLevel level) {
    switch (level) {
        case RF4S_LOG_DEBUG: return RF4S::LogLevel::DEBUG;
        case RF4S_LOG_INFO:  return RF4S::LogLevel::INFO;
        case RF4S_LOG_WARN:  return RF4S::LogLevel::WARN;
        case RF4S_LOG_ERROR: return RF4S::LogLevel::ERROR_LEVEL;
        default: return RF4S::LogLevel::INFO;
    }
}

extern "C" {

// ============================================================================
// Hook Management API Implementation
// ============================================================================

bool rf4s_create_hook(const char* hook_name, void* target, void* detour, void** original) {
    if (!hook_name || !target || !detour) {
        return false;
    }
    
    RF4S::HookManager& hookManager = RF4S::ScriptLoader::GetInstance().GetHookManager();
    return hookManager.CreateHook(hook_name, target, detour, original);
}

bool rf4s_enable_hook(const char* hook_name) {
    if (!hook_name) {
        return false;
    }
    
    RF4S::HookManager& hookManager = RF4S::ScriptLoader::GetInstance().GetHookManager();
    return hookManager.EnableHook(hook_name);
}

bool rf4s_disable_hook(const char* hook_name) {
    if (!hook_name) {
        return false;
    }
    
    RF4S::HookManager& hookManager = RF4S::ScriptLoader::GetInstance().GetHookManager();
    return hookManager.DisableHook(hook_name);
}

bool rf4s_remove_hook(const char* hook_name) {
    if (!hook_name) {
        return false;
    }
    
    RF4S::HookManager& hookManager = RF4S::ScriptLoader::GetInstance().GetHookManager();
    return hookManager.RemoveHook(hook_name);
}

bool rf4s_is_hook_enabled(const char* hook_name) {
    if (!hook_name) {
        return false;
    }
    
    RF4S::HookManager& hookManager = RF4S::ScriptLoader::GetInstance().GetHookManager();
    return hookManager.IsHookEnabled(hook_name);
}

// ============================================================================
// Logging API Implementation
// ============================================================================

void rf4s_log(RF4SLogLevel level, const char* format, ...) {
    if (!format) {
        return;
    }
    
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    RF4S::Logger& logger = RF4S::ScriptLoader::GetInstance().GetLogger();
    logger.Log(ConvertLogLevel(level), "%s", buffer);
}

void rf4s_set_log_level(RF4SLogLevel level) {
    RF4S::Logger& logger = RF4S::ScriptLoader::GetInstance().GetLogger();
    logger.SetLogLevel(ConvertLogLevel(level));
}

// ============================================================================
// Configuration API Implementation
// ============================================================================

const char* rf4s_get_config_string(const char* key, const char* default_value) {
    if (!key) {
        return default_value ? default_value : "";
    }
    
    RF4S::ConfigManager& config = RF4S::ScriptLoader::GetInstance().GetConfigManager();
    std::string value = config.GetString(key, default_value ? default_value : "");
    return CacheString(value);
}

bool rf4s_get_config_bool(const char* key, bool default_value) {
    if (!key) {
        return default_value;
    }
    
    RF4S::ConfigManager& config = RF4S::ScriptLoader::GetInstance().GetConfigManager();
    return config.GetBool(key, default_value);
}

int rf4s_get_config_int(const char* key, int default_value) {
    if (!key) {
        return default_value;
    }
    
    RF4S::ConfigManager& config = RF4S::ScriptLoader::GetInstance().GetConfigManager();
    return config.GetInt(key, default_value);
}

double rf4s_get_config_double(const char* key, double default_value) {
    if (!key) {
        return default_value;
    }
    
    RF4S::ConfigManager& config = RF4S::ScriptLoader::GetInstance().GetConfigManager();
    return config.GetDouble(key, default_value);
}

void rf4s_set_config_string(const char* key, const char* value) {
    if (!key || !value) {
        return;
    }
    
    RF4S::ConfigManager& config = RF4S::ScriptLoader::GetInstance().GetConfigManager();
    config.SetString(key, value);
}

void rf4s_set_config_bool(const char* key, bool value) {
    if (!key) {
        return;
    }
    
    RF4S::ConfigManager& config = RF4S::ScriptLoader::GetInstance().GetConfigManager();
    config.SetBool(key, value);
}

void rf4s_set_config_int(const char* key, int value) {
    if (!key) {
        return;
    }
    
    RF4S::ConfigManager& config = RF4S::ScriptLoader::GetInstance().GetConfigManager();
    config.SetInt(key, value);
}

void rf4s_set_config_double(const char* key, double value) {
    if (!key) {
        return;
    }
    
    RF4S::ConfigManager& config = RF4S::ScriptLoader::GetInstance().GetConfigManager();
    config.SetDouble(key, value);
}

bool rf4s_save_config(void) {
    RF4S::ConfigManager& config = RF4S::ScriptLoader::GetInstance().GetConfigManager();
    return config.SaveConfig();
}

bool rf4s_has_config_key(const char* key) {
    if (!key) {
        return false;
    }
    
    RF4S::ConfigManager& config = RF4S::ScriptLoader::GetInstance().GetConfigManager();
    return config.HasKey(key);
}

// ============================================================================
// UI API Implementation (Placeholder)
// ============================================================================

bool rf4s_ui_add_window(const char* title, RF4SUICallback render_callback) {
    // TODO: Implement ImGui integration
    if (!title || !render_callback) {
        return false;
    }
    
    rf4s_log(RF4S_LOG_WARN, "UI system not yet implemented");
    return false;
}

void rf4s_ui_remove_window(const char* title) {
    // TODO: Implement ImGui integration
    if (!title) {
        return;
    }
    
    rf4s_log(RF4S_LOG_WARN, "UI system not yet implemented");
}

bool rf4s_ui_is_available(void) {
    // TODO: Return true when ImGui is implemented
    return false;
}

// ============================================================================
// Utility API Implementation
// ============================================================================

uintptr_t rf4s_get_module_base(void) {
    return RF4S::ScriptLoader::GetInstance().GetModuleBase();
}

const char* rf4s_get_version(void) {
    return RF4S::ScriptLoader::GetInstance().GetVersion();
}

const char* rf4s_get_api_version(void) {
    return RF4S::ScriptLoader::GetInstance().GetAPIVersion();
}

} // extern "C"