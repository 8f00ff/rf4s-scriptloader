#pragma once

#include "../../api/rf4s_types.h"
#include "../../api/rf4s_api.h"
#include <cstdint>

namespace RF4S {

class EveryoneIsBiMod {
public:
    static EveryoneIsBiMod& GetInstance();
    
    bool Initialize();
    void Shutdown();
    void LoadConfig();
    
    // Hook implementations
    static int HookedFriendshipCheck(uint64_t charId);
    static void HookedConfessionFunc(
        void* param_1, // RCX: context/state object
        void* param_2, // RDX: buffer/array
        void* param_3, // R8: another buffer/array
        void* param_4, // R9: pointer to a value
        char* param_5  // [RBP+0x50]: string/buffer being processed
    );
    
private:
    EveryoneIsBiMod() = default;
    ~EveryoneIsBiMod() = default;
    
    EveryoneIsBiMod(const EveryoneIsBiMod&) = delete;
    EveryoneIsBiMod& operator=(const EveryoneIsBiMod&) = delete;
    
    // Original function pointers
    using FnFriend = int(*)(uint64_t);
    using FnConfessFunc = void(*)(void*, void*, void*, void*, char*);
    
    static FnFriend originalFriendshipCheck;
    static FnConfessFunc originalConfessionFunc;
    
    // Configuration
    bool enabled = true;
    bool debugLogging = false;
    bool genderModificationEnabled = true;
    
    // Game constants
    static constexpr uintptr_t OFF_FRIENDSHIP_CHECK = 0x00227380;
    static constexpr uintptr_t OFF_CONFESSION_FUNC = 0x0023E1C0;
    
    static constexpr uint32_t MALE_CONFESSION_EVENT = 0x16070007;
    static constexpr uint32_t FEMALE_CONFESSION_EVENT = 0x07008207;
    
    // Marriageable character IDs that should show hearts
    static constexpr uint32_t MARRIAGEABLE_CHARS[] = {
        1, 2, 4, 5, 7, 8, 9, 11, 13, 15, 17, 20
    };
    
    // Helper methods
    bool IsMarriageableChar(uint32_t charId) const;
    bool TryReadEventCode(void* ptr, uint32_t& eventCode) const;
    bool TryModifyGenderFlag(void* basePtr, uint32_t eventCode) const;
    
    bool initialized = false;
};

} // namespace RF4S

// C API for mod loading
extern "C" {
    __declspec(dllexport) RF4SModInfo* GetModInfo();
}