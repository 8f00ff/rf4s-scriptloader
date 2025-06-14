#include "config_manager.h"
#include "logger.h"
#include <fstream>
#include <filesystem>

namespace RF4S {

ConfigManager& ConfigManager::GetInstance() {
    static ConfigManager instance;
    return instance;
}

ConfigManager::~ConfigManager() {
    Shutdown();
}

bool ConfigManager::Initialize(const std::string& configPath) {
    if (initialized) {
        RF4S_LOG_WARN("ConfigManager already initialized");
        return true;
    }
    
    this->configPath = configPath;
    
    // Create config directory if it doesn't exist
    std::filesystem::path configDir = std::filesystem::path(configPath).parent_path();
    if (!configDir.empty() && !std::filesystem::exists(configDir)) {
        try {
            std::filesystem::create_directories(configDir);
            RF4S_LOG_INFO("Created config directory: %s", configDir.string().c_str());
        } catch (const std::exception& e) {
            RF4S_LOG_ERROR("Failed to create config directory: %s", e.what());
            return false;
        }
    }
    
    config = std::make_unique<toml::table>();
    
    // Mark as initialized now that config table is created
    initialized = true;
    
    // Try to load existing config
    if (std::filesystem::exists(configPath)) {
        if (!LoadConfig()) {
            RF4S_LOG_WARN("Failed to load existing config, using defaults");
        }
    } else {
        RF4S_LOG_INFO("Config file doesn't exist, will create with defaults");
        
        // Set default values
        SetString("core.log_level", "info");
        SetBool("core.enable_console", true);
        // Save default config
        if (!SaveConfig()) {
            RF4S_LOG_ERROR("Failed to save default config");
            initialized = false;  // Reset on failure
            return false;
        }
    }
    RF4S_LOG_INFO("ConfigManager initialized (config: %s)", configPath.c_str());
    return true;
}

void ConfigManager::Shutdown() {
    if (initialized) {
        RF4S_LOG_INFO("ConfigManager shutting down");
        config.reset();
        initialized = false;
    }
}

bool ConfigManager::LoadConfig() {
    if (!initialized) {
        RF4S_LOG_ERROR("ConfigManager not initialized");
        return false;
    }
    
    try {
        *config = toml::parse_file(configPath);
        RF4S_LOG_INFO("Config loaded successfully from %s", configPath.c_str());
        return true;
    } catch (const toml::parse_error& e) {
        RF4S_LOG_ERROR("Failed to parse config file: %s", e.what());
        return false;
    } catch (const std::exception& e) {
        RF4S_LOG_ERROR("Failed to load config file: %s", e.what());
        return false;
    }
}

bool ConfigManager::SaveConfig() {
    if (!initialized) {
        RF4S_LOG_ERROR("ConfigManager not initialized");
        return false;
    }
    
    try {
        std::ofstream file(configPath);
        if (!file.is_open()) {
            RF4S_LOG_ERROR("Failed to open config file for writing: %s", configPath.c_str());
            return false;
        }
        
        file << *config;
        file.close();
        
        RF4S_LOG_INFO("Config saved successfully to %s", configPath.c_str());
        return true;
    } catch (const std::exception& e) {
        RF4S_LOG_ERROR("Failed to save config file: %s", e.what());
        return false;
    }
}

std::string ConfigManager::GetString(const std::string& key, const std::string& defaultValue) const {
    if (!initialized) {
        return defaultValue;
    }
    
    auto [section, keyName] = SplitKey(key);
    
    if (section.empty()) {
        // Top-level key
        auto value = (*config)[keyName].value<std::string>();
        return value ? *value : defaultValue;
    } else {
        // Nested key
        const toml::table* table = GetNestedTable(section);
        if (table) {
            auto value = (*table)[keyName].value<std::string>();
            return value ? *value : defaultValue;
        }
        return defaultValue;
    }
}

bool ConfigManager::GetBool(const std::string& key, bool defaultValue) const {
    if (!initialized) {
        return defaultValue;
    }
    
    auto [section, keyName] = SplitKey(key);
    
    if (section.empty()) {
        auto value = (*config)[keyName].value<bool>();
        return value ? *value : defaultValue;
    } else {
        const toml::table* table = GetNestedTable(section);
        if (table) {
            auto value = (*table)[keyName].value<bool>();
            return value ? *value : defaultValue;
        }
        return defaultValue;
    }
}

int ConfigManager::GetInt(const std::string& key, int defaultValue) const {
    if (!initialized) {
        return defaultValue;
    }
    
    auto [section, keyName] = SplitKey(key);
    
    if (section.empty()) {
        auto value = (*config)[keyName].value<int64_t>();
        return value ? static_cast<int>(*value) : defaultValue;
    } else {
        const toml::table* table = GetNestedTable(section);
        if (table) {
            auto value = (*table)[keyName].value<int64_t>();
            return value ? static_cast<int>(*value) : defaultValue;
        }
        return defaultValue;
    }
}

double ConfigManager::GetDouble(const std::string& key, double defaultValue) const {
    if (!initialized) {
        return defaultValue;
    }
    
    auto [section, keyName] = SplitKey(key);
    
    if (section.empty()) {
        auto value = (*config)[keyName].value<double>();
        return value ? *value : defaultValue;
    } else {
        const toml::table* table = GetNestedTable(section);
        if (table) {
            auto value = (*table)[keyName].value<double>();
            return value ? *value : defaultValue;
        }
        return defaultValue;
    }
}

void ConfigManager::SetString(const std::string& key, const std::string& value) {
    if (!initialized) {
        return;
    }
    
    auto [section, keyName] = SplitKey(key);
    
    if (section.empty()) {
        config->insert_or_assign(keyName, value);
    } else {
        toml::table* table = GetNestedTable(section, true);
        if (table) {
            table->insert_or_assign(keyName, value);
        }
    }
}

void ConfigManager::SetBool(const std::string& key, bool value) {
    if (!initialized) {
        return;
    }
    
    auto [section, keyName] = SplitKey(key);
    
    if (section.empty()) {
        config->insert_or_assign(keyName, value);
    } else {
        toml::table* table = GetNestedTable(section, true);
        if (table) {
            table->insert_or_assign(keyName, value);
        }
    }
}

void ConfigManager::SetInt(const std::string& key, int value) {
    if (!initialized) {
        return;
    }
    
    auto [section, keyName] = SplitKey(key);
    
    if (section.empty()) {
        config->insert_or_assign(keyName, static_cast<int64_t>(value));
    } else {
        toml::table* table = GetNestedTable(section, true);
        if (table) {
            table->insert_or_assign(keyName, static_cast<int64_t>(value));
        }
    }
}

void ConfigManager::SetDouble(const std::string& key, double value) {
    if (!initialized) {
        return;
    }
    
    auto [section, keyName] = SplitKey(key);
    
    if (section.empty()) {
        config->insert_or_assign(keyName, value);
    } else {
        toml::table* table = GetNestedTable(section, true);
        if (table) {
            table->insert_or_assign(keyName, value);
        }
    }
}

bool ConfigManager::HasKey(const std::string& key) const {
    if (!initialized) {
        return false;
    }
    
    auto [section, keyName] = SplitKey(key);
    
    if (section.empty()) {
        return config->contains(keyName);
    } else {
        const toml::table* table = GetNestedTable(section);
        return table && table->contains(keyName);
    }
}

toml::table* ConfigManager::GetNestedTable(const std::string& key, bool createIfMissing) {
    if (!config) {
        return nullptr;
    }
    
    auto node = config->get(key);
    if (node && node->is_table()) {
        return node->as_table();
    } else if (createIfMissing) {
        config->insert_or_assign(key, toml::table{});
        return config->get(key)->as_table();
    }
    
    return nullptr;
}

const toml::table* ConfigManager::GetNestedTable(const std::string& key) const {
    if (!config) {
        return nullptr;
    }
    
    auto node = config->get(key);
    return (node && node->is_table()) ? node->as_table() : nullptr;
}

std::pair<std::string, std::string> ConfigManager::SplitKey(const std::string& key) const {
    size_t dotPos = key.find('.');
    if (dotPos == std::string::npos) {
        return {"", key};
    }
    
    return {key.substr(0, dotPos), key.substr(dotPos + 1)};
}

} // namespace RF4S