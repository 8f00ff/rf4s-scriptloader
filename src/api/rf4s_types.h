#pragma once

#include <cstdint>

// Log levels for the public API (global scope for C compatibility)
typedef enum {
    RF4S_LOG_DEBUG = 0,
    RF4S_LOG_INFO = 1,
    RF4S_LOG_WARN = 2,
    RF4S_LOG_ERROR = 3
} RF4SLogLevel;

// Mod information structure (global scope for C compatibility)
typedef struct {
    const char* name;
    const char* version;
    const char* author;
    const char* description;
    const char* license;
    const char* homepage;
    const char* api_version;
    
    // Mod lifecycle callbacks
    bool (*initialize)(void);
    void (*shutdown)(void);
    void (*on_config_changed)(void);
} RF4SModInfo;

// Hook callback types
typedef void* (*RF4SHookCallback)(void);

// Configuration value types
typedef enum {
    RF4S_CONFIG_STRING,
    RF4S_CONFIG_BOOL,
    RF4S_CONFIG_INT,
    RF4S_CONFIG_DOUBLE
} RF4SConfigType;

// UI callback type for ImGui windows
typedef void (*RF4SUICallback)(void);

namespace RF4S {
    // Import types into namespace for C++ code
    using ::RF4SLogLevel;
    using ::RF4SModInfo;
    using ::RF4SHookCallback;
    using ::RF4SConfigType;
    using ::RF4SUICallback;
} // namespace RF4S