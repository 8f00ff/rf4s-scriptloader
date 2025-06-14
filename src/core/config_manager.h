#pragma once

#include <string>
#include <memory>
#include "../../extern/tomlplusplus/toml.hpp"

namespace RF4S {

class ConfigManager {
public:
    static ConfigManager& GetInstance();
    
    bool Initialize(const std::string& configPath);
    void Shutdown();
    
    bool LoadConfig();
    bool SaveConfig();
    
    // Configuration getters
    std::string GetString(const std::string& key, const std::string& defaultValue = "") const;
    bool GetBool(const std::string& key, bool defaultValue = false) const;
    int GetInt(const std::string& key, int defaultValue = 0) const;
    double GetDouble(const std::string& key, double defaultValue = 0.0) const;
    
    // Configuration setters
    void SetString(const std::string& key, const std::string& value);
    void SetBool(const std::string& key, bool value);
    void SetInt(const std::string& key, int value);
    void SetDouble(const std::string& key, double value);
    
    // Check if key exists
    bool HasKey(const std::string& key) const;
    
    // Get the config file path
    const std::string& GetConfigPath() const { return configPath; }
    
private:
    ConfigManager() = default;
    ~ConfigManager();
    
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    std::string configPath;
    std::unique_ptr<toml::table> config;
    bool initialized = false;
    
    // Helper methods for nested key access (e.g., "core.log_level")
    toml::table* GetNestedTable(const std::string& key, bool createIfMissing = false);
    const toml::table* GetNestedTable(const std::string& key) const;
    std::pair<std::string, std::string> SplitKey(const std::string& key) const;
};

} // namespace RF4S