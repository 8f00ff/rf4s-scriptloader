#include "bi_mod.h"
#include <windows.h>

namespace RF4S {

// Static member definitions
EveryoneIsBiMod::FnFriend EveryoneIsBiMod::originalFriendshipCheck = nullptr;
EveryoneIsBiMod::FnConfessFunc EveryoneIsBiMod::originalConfessionFunc = nullptr;

EveryoneIsBiMod& EveryoneIsBiMod::GetInstance() {
    static EveryoneIsBiMod instance;
    return instance;
}

bool EveryoneIsBiMod::Initialize() {
    if (initialized) {
        rf4s_log(RF4S_LOG_WARN, "Everyone is Bi mod already initialized");
        return true;
    }
    
    rf4s_log(RF4S_LOG_INFO, "Initializing Everyone is Bi mod");
    
    // Load configuration
    LoadConfig();
    
    if (!enabled) {
        rf4s_log(RF4S_LOG_INFO, "Everyone is Bi mod is disabled in config");
        return true;
    }
    
    // Get module base address
    uintptr_t base = rf4s_get_module_base();
    if (base == 0) {
        rf4s_log(RF4S_LOG_ERROR, "Failed to get module base address");
        return false;
    }
    
    // Create friendship check hook
    void* friendshipTarget = reinterpret_cast<void*>(base + OFF_FRIENDSHIP_CHECK);
    if (!rf4s_create_hook("bi_friendship_check", friendshipTarget, 
                         reinterpret_cast<void*>(&HookedFriendshipCheck),
                         reinterpret_cast<void**>(&originalFriendshipCheck))) {
        rf4s_log(RF4S_LOG_ERROR, "Failed to create friendship hook");
        return false;
    }
    
    // Create confession hook
    void* confessionTarget = reinterpret_cast<void*>(base + OFF_CONFESSION_FUNC);
    if (!rf4s_create_hook("bi_confession", confessionTarget,
                         reinterpret_cast<void*>(&HookedConfessionFunc),
                         reinterpret_cast<void**>(&originalConfessionFunc))) {
        rf4s_log(RF4S_LOG_ERROR, "Failed to create confession hook");
        return false;
    }
    
    // Enable hooks
    if (!rf4s_enable_hook("bi_friendship_check")) {
        rf4s_log(RF4S_LOG_ERROR, "Failed to enable friendship hook");
        return false;
    }
    
    if (!rf4s_enable_hook("bi_confession")) {
        rf4s_log(RF4S_LOG_ERROR, "Failed to enable confession hook");
        return false;
    }
    
    initialized = true;
    rf4s_log(RF4S_LOG_INFO, "Everyone is Bi mod initialized successfully");
    rf4s_log(RF4S_LOG_INFO, "Gender modification: %s", genderModificationEnabled ? "enabled" : "disabled");
    rf4s_log(RF4S_LOG_INFO, "Debug logging: %s", debugLogging ? "enabled" : "disabled");
    
    return true;
}

void EveryoneIsBiMod::Shutdown() {
    if (!initialized) {
        return;
    }
    
    rf4s_log(RF4S_LOG_INFO, "Shutting down Everyone is Bi mod");
    
    // Remove hooks
    rf4s_remove_hook("bi_friendship_check");
    rf4s_remove_hook("bi_confession");
    
    originalFriendshipCheck = nullptr;
    originalConfessionFunc = nullptr;
    initialized = false;
    
    rf4s_log(RF4S_LOG_INFO, "Everyone is Bi mod shutdown complete");
}

void EveryoneIsBiMod::LoadConfig() {
    enabled = rf4s_get_config_bool("everyone_is_bi.enabled", true);
    debugLogging = rf4s_get_config_bool("everyone_is_bi.logging.debug_mode", false);
    genderModificationEnabled = rf4s_get_config_bool("everyone_is_bi.gender_modification", true);
    
    rf4s_log(RF4S_LOG_INFO, "Everyone is Bi mod config loaded");
}

int EveryoneIsBiMod::HookedFriendshipCheck(uint64_t charId) {
    EveryoneIsBiMod& mod = GetInstance();
    uint32_t charIdInt = static_cast<uint32_t>(charId);
    bool showHeart = mod.IsMarriageableChar(charIdInt);
    
    if (mod.debugLogging) {
        rf4s_log(RF4S_LOG_DEBUG, "[Everyone is Bi] ID %u → %s", charIdInt, showHeart ? "<3" : ":)");
    }
    
    return showHeart ? 1 : 0;
}

void EveryoneIsBiMod::HookedConfessionFunc(
    void* param_1, // RCX: context/state object
    void* param_2, // RDX: buffer/array
    void* param_3, // R8: another buffer/array
    void* param_4, // R9: pointer to a value
    char* param_5  // [RBP+0x50]: string/buffer being processed
) {
    EveryoneIsBiMod& mod = GetInstance();
    
    if (mod.debugLogging) {
        rf4s_log(RF4S_LOG_DEBUG, "[Everyone is Bi] Confession function called");
    }
    
    bool flagModified = false;
    uint32_t eventCode = 0;
    
    // Try to find the event code in the parameters
    if (mod.TryReadEventCode(param_5, eventCode) || mod.TryReadEventCode(param_1, eventCode)) {
        if (mod.debugLogging) {
            rf4s_log(RF4S_LOG_DEBUG, "[Everyone is Bi] Found event code: 0x%08X", eventCode);
        }
        
        // Check if this is a confession event and try to modify the gender flag
        if (eventCode == MALE_CONFESSION_EVENT || eventCode == FEMALE_CONFESSION_EVENT) {
            // Try param_1 first (most likely location for the flag)
            flagModified = mod.TryModifyGenderFlag(param_1, eventCode);
            
            // If that didn't work, try param_2
            if (!flagModified) {
                flagModified = mod.TryModifyGenderFlag(param_2, eventCode);
            }
        }
    }
    
    // Call the original function
    if (originalConfessionFunc) {
        try {
            originalConfessionFunc(param_1, param_2, param_3, param_4, param_5);
            if (mod.debugLogging) {
                rf4s_log(RF4S_LOG_DEBUG, "[Everyone is Bi] Original function called successfully");
            }
        } catch (...) {
            rf4s_log(RF4S_LOG_ERROR, "[Everyone is Bi] Exception in original function");
        }
    } else {
        rf4s_log(RF4S_LOG_ERROR, "[Everyone is Bi] originalConfessionFunc is null");
    }
}

bool EveryoneIsBiMod::IsMarriageableChar(uint32_t charId) const {
    for (uint32_t id : MARRIAGEABLE_CHARS) {
        if (charId == id) {
            return true;
        }
    }
    return false;
}

bool EveryoneIsBiMod::TryReadEventCode(void* ptr, uint32_t& eventCode) const {
    if (!ptr) {
        return false;
    }
    
    try {
        eventCode = *reinterpret_cast<uint32_t*>(ptr);
        return true;
    } catch (...) {
        return false;
    }
}

bool EveryoneIsBiMod::TryModifyGenderFlag(void* basePtr, uint32_t eventCode) const {
    if (!basePtr || !genderModificationEnabled) {
        return false;
    }
    
    try {
        uint8_t* flagPtr = reinterpret_cast<uint8_t*>(basePtr) + 0x40;
        uint8_t oldFlag = *flagPtr;
        
        if (debugLogging) {
            rf4s_log(RF4S_LOG_DEBUG, "[Everyone is Bi] Event=0x%08X, flag before=%u", eventCode, oldFlag);
        }
        
        // Modify flag based on event code and current value
        if (eventCode == MALE_CONFESSION_EVENT && oldFlag == 1) {
            *flagPtr = 0;
            rf4s_log(RF4S_LOG_INFO, "[Everyone is Bi] Modified flag for male confession");
            return true;
        } else if (eventCode == FEMALE_CONFESSION_EVENT && oldFlag == 0) {
            *flagPtr = 1;
            rf4s_log(RF4S_LOG_INFO, "[Everyone is Bi] Modified flag for female confession");
            return true;
        }
        
        if (debugLogging) {
            rf4s_log(RF4S_LOG_DEBUG, "[Everyone is Bi] Flag after=%u", *flagPtr);
        }
    } catch (...) {
        rf4s_log(RF4S_LOG_ERROR, "[Everyone is Bi] Exception while modifying gender flag");
    }
    
    return false;
}

} // namespace RF4S

// C API implementation
extern "C" {

__declspec(dllexport) RF4SModInfo* GetModInfo() {
    static RF4SModInfo info = {
        .name = "Everyone is Bi",
        .version = "2.0.0",
        .author = "RF4S Community",
        .description = "Enables same-sex relationships in RF4S",
        .license = "GPL-3.0",
        .homepage = "https://github.com/8f00ff/rf4s-scriptloader",
        .api_version = "1.0.0",
        .initialize = []() -> bool {
            return RF4S::EveryoneIsBiMod::GetInstance().Initialize();
        },
        .shutdown = []() -> void {
            RF4S::EveryoneIsBiMod::GetInstance().Shutdown();
        },
        .on_config_changed = []() -> void {
            RF4S::EveryoneIsBiMod::GetInstance().LoadConfig();
        }
    };
    return &info;
}

} // extern "C"