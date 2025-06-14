#pragma once

#include "rf4s_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Hook Management API
// ============================================================================

/**
 * Create a new hook with the specified name
 * @param hook_name Unique name for the hook
 * @param target Target function address to hook
 * @param detour Detour function to call instead
 * @param original Pointer to store the original function address
 * @return true if hook was created successfully, false otherwise
 */
bool rf4s_create_hook(const char* hook_name, void* target, void* detour, void** original);

/**
 * Enable a previously created hook
 * @param hook_name Name of the hook to enable
 * @return true if hook was enabled successfully, false otherwise
 */
bool rf4s_enable_hook(const char* hook_name);

/**
 * Disable a hook without removing it
 * @param hook_name Name of the hook to disable
 * @return true if hook was disabled successfully, false otherwise
 */
bool rf4s_disable_hook(const char* hook_name);

/**
 * Remove a hook completely
 * @param hook_name Name of the hook to remove
 * @return true if hook was removed successfully, false otherwise
 */
bool rf4s_remove_hook(const char* hook_name);

/**
 * Check if a hook exists and is enabled
 * @param hook_name Name of the hook to check
 * @return true if hook exists and is enabled, false otherwise
 */
bool rf4s_is_hook_enabled(const char* hook_name);

// ============================================================================
// Logging API
// ============================================================================

/**
 * Log a message with the specified level
 * @param level Log level (RF4S_LOG_DEBUG, RF4S_LOG_INFO, RF4S_LOG_WARN, RF4S_LOG_ERROR)
 * @param format Printf-style format string
 * @param ... Format arguments
 */
void rf4s_log(RF4SLogLevel level, const char* format, ...);

/**
 * Set the minimum log level
 * @param level Minimum log level to display
 */
void rf4s_set_log_level(RF4SLogLevel level);

// ============================================================================
// Configuration API
// ============================================================================

/**
 * Get a string configuration value
 * @param key Configuration key (supports nested keys like "section.key")
 * @param default_value Default value if key doesn't exist
 * @return Configuration value or default_value
 */
const char* rf4s_get_config_string(const char* key, const char* default_value);

/**
 * Get a boolean configuration value
 * @param key Configuration key
 * @param default_value Default value if key doesn't exist
 * @return Configuration value or default_value
 */
bool rf4s_get_config_bool(const char* key, bool default_value);

/**
 * Get an integer configuration value
 * @param key Configuration key
 * @param default_value Default value if key doesn't exist
 * @return Configuration value or default_value
 */
int rf4s_get_config_int(const char* key, int default_value);

/**
 * Get a double configuration value
 * @param key Configuration key
 * @param default_value Default value if key doesn't exist
 * @return Configuration value or default_value
 */
double rf4s_get_config_double(const char* key, double default_value);

/**
 * Set a string configuration value
 * @param key Configuration key
 * @param value Value to set
 */
void rf4s_set_config_string(const char* key, const char* value);

/**
 * Set a boolean configuration value
 * @param key Configuration key
 * @param value Value to set
 */
void rf4s_set_config_bool(const char* key, bool value);

/**
 * Set an integer configuration value
 * @param key Configuration key
 * @param value Value to set
 */
void rf4s_set_config_int(const char* key, int value);

/**
 * Set a double configuration value
 * @param key Configuration key
 * @param value Value to set
 */
void rf4s_set_config_double(const char* key, double value);

/**
 * Save the current configuration to disk
 * @return true if configuration was saved successfully, false otherwise
 */
bool rf4s_save_config(void);

/**
 * Check if a configuration key exists
 * @param key Configuration key to check
 * @return true if key exists, false otherwise
 */
bool rf4s_has_config_key(const char* key);

// ============================================================================
// UI API (ImGui Integration)
// ============================================================================

/**
 * Add a new ImGui window
 * @param title Window title (must be unique)
 * @param render_callback Function to call for rendering the window content
 * @return true if window was added successfully, false otherwise
 */
bool rf4s_ui_add_window(const char* title, RF4SUICallback render_callback);

/**
 * Remove an ImGui window
 * @param title Title of the window to remove
 */
void rf4s_ui_remove_window(const char* title);

/**
 * Check if UI system is available
 * @return true if UI system is initialized and available, false otherwise
 */
bool rf4s_ui_is_available(void);

// ============================================================================
// Utility API
// ============================================================================

/**
 * Get the base address of the main executable
 * @return Base address of the main executable
 */
uintptr_t rf4s_get_module_base(void);

/**
 * Get the RF4S ScriptLoader version
 * @return Version string
 */
const char* rf4s_get_version(void);

/**
 * Get the API version
 * @return API version string
 */
const char* rf4s_get_api_version(void);

#ifdef __cplusplus
}
#endif