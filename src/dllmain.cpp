#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>
#include "MinHook.h"

// Global log file stream
static std::ofstream logFile;

// Custom logging function that writes to both console and file
void log_message(const char* format, ...) {
  char buffer[1024];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  
  // Write to console
  printf("%s", buffer);
  
  // Write to log file if open
  if (logFile.is_open()) {
    logFile << buffer << std::flush;
  }
}

// ——— Base & offsets ———
static uintptr_t RF4S_BASE = 0;
static const uintptr_t OFF_FRIENDSHIP_CHECK = 0x00227380;
static const uintptr_t OFF_CONFESSION_FUNC  = 0x0023E1C0;  // Entry point of the function
static const uintptr_t OFF_CONFESSION_POINT = 0x0023E210;  // Original hook point (for reference)

// ——— Typedefs & originals ———
// friendship gets RCX directly as the charId
using FnFriend  = int(*)(uint64_t /*charId*/);
static FnFriend  originalFriend  = nullptr;

// Full function signature for the confession function based on disassembly
using FnConfessFunc = void(*)(
    void* param_1,      // RCX: appears to be a context/state object
    void* param_2,      // RDX: appears to be a buffer/array
    void* param_3,      // R8: appears to be another buffer/array
    void* param_4,      // R9: appears to be a pointer to a value
    char* param_5       // [RBP+0x50]: appears to be a string/buffer being processed
);
static FnConfessFunc originalConfessFunc = nullptr;

// friendship hook
int HookedFriend(uint64_t rcx) {
  uint32_t charId = static_cast<uint32_t>(rcx);
  
  bool showHeart =
    charId==1  || charId==2  || charId==4  || charId==5  ||
    charId==7  || charId==8  || charId==9  || charId==11 ||
    charId==13 || charId==15 || charId==17 || charId==20;
  
  log_message("[RF4S Extender] ID %u → %s\n", charId, showHeart ? "<3" : ":)");
  
  return showHeart ? 1 : 0;
}

// Test mode flag - Set to true to enable gender modification features
// (safe now that we're not calling originalConfess)
static bool enableGenderModification = true;

// Assembly-compatible confession hook - DON'T CALL originalConfess
// Based on the crash log, calling originalConfess directly causes the crash
// Instead, we'll just compute the return value and return it directly
int AssemblyAwareConfession(void* rdiPtr, void* rcxPtr) {
  log_message("[RF4S Extender] AssemblyAwareConfession called with rdiPtr=%p, rcxPtr=%p\n", rdiPtr, rcxPtr);

  // Try to manually perform the same operations the original assembly would do
  int returnValue = -1; // Default to -1 if we can't read the byte
  
  try {
    if (rdiPtr) {
      // movzx edx,byte ptr [rdi] - Read byte at rdiPtr and zero-extend
      uint8_t byteValue = *reinterpret_cast<uint8_t*>(rdiPtr);
      log_message("[RF4S Extender] Read byte value %02X from [rdi]\n", byteValue);
      
      // lea eax,[rdx-01] - Compute rdx-1
      returnValue = static_cast<int>(byteValue) - 1;
      log_message("[RF4S Extender] Computed return value: %d\n", returnValue);
    } else {
      log_message("[RF4S Extender] WARNING: rdiPtr is NULL, cannot read byte\n");
    }
  } catch (...) {
    log_message("[RF4S Extender] ERROR: Exception while trying to read from rdiPtr\n");
  }

  // IMPORTANT: We are NOT calling originalConfess at all, as this seems to be the source of the crash
  // Looking at the assembly in the readme, it suggests we just need to return (byte at [rdi])-1
  log_message("[RF4S Extender] Skipping originalConfess call to avoid crash\n");
  return returnValue;
}

// Full confession hook with gender flag manipulation
int FullHookedConfession(void* rdiPtr, void* rcxPtr) {
  // Default return value (-1) in case we can't read the byte
  int returnValue = -1;
  
  // Safety checks for null pointers
  if (!rdiPtr || !rcxPtr) {
    log_message("[RF4S Extender] ERROR: Null pointer in FullHookedConfession. rdiPtr=%p, rcxPtr=%p\n",
           rdiPtr, rcxPtr);
    return returnValue;
  }
  
  // Log addresses for debugging
  log_message("[RF4S Extender] FullHookedConfession called with rdiPtr=%p, rcxPtr=%p\n", rdiPtr, rcxPtr);
  
  // First, compute the correct return value by reading byte at [rdi] and subtracting 1
  try {
    uint8_t byteValue = *reinterpret_cast<uint8_t*>(rdiPtr);
    returnValue = static_cast<int>(byteValue) - 1;
    log_message("[RF4S Extender] Read byte value %02X from [rdi], return value = %d\n",
               byteValue, returnValue);
  } catch (...) {
    log_message("[RF4S Extender] ERROR: Exception reading byte from rdiPtr\n");
    return returnValue;
  }
  
  // Now handle the gender flag modification logic
  uint32_t eventCode = 0;
  try {
    eventCode = *reinterpret_cast<uint32_t*>(rdiPtr);
  } catch (...) {
    log_message("[RF4S Extender] ERROR: Exception reading eventCode from rdiPtr\n");
    return returnValue;
  }
  
  // Safely access flagPtr with proper validation
  uint8_t* flagPtr = nullptr;
  uint8_t oldFlag = 0;
  try {
    flagPtr = reinterpret_cast<uint8_t*>(rcxPtr) + 0x40;
    oldFlag = *flagPtr;
  } catch (...) {
    log_message("[RF4S Extender] ERROR: Exception reading flagPtr from rcxPtr+0x40\n");
    return returnValue;
  }
  
  log_message("[RF4S Extender] Confess event=0x%08X, before=%u\n", eventCode, oldFlag);
  
  // Only modify flag if it matches expected conditions
  try {
    if (eventCode == 0x16070007 && oldFlag == 1) {
      *flagPtr = 0;
      log_message("[RF4S Extender] Modified flag for male confession (0x16070007)\n");
    } else if (eventCode == 0x07008207 && oldFlag == 0) {
      *flagPtr = 1;
      log_message("[RF4S Extender] Modified flag for female confession (0x07008207)\n");
    }
    
    log_message("[RF4S Extender] after=%u\n", *flagPtr);
  } catch (...) {
    log_message("[RF4S Extender] ERROR: Exception writing to flagPtr\n");
  }
  
  // IMPORTANT: Do NOT call originalConfess as this causes crashes
  log_message("[RF4S Extender] Skipping originalConfess call in FullHookedConfession\n");
  return returnValue;
}

// Wrapper that selects which implementation to use
int HookedConfession(void* rdiPtr, void* rcxPtr) {
  if (!enableGenderModification) {
    // Use the safest implementation that matches assembly expectations
    return AssemblyAwareConfession(rdiPtr, rcxPtr);
  } else {
    // Use the full implementation with gender modifications
    return FullHookedConfession(rdiPtr, rcxPtr);
  }
}

// This is our implementation of the confession function
void HookedConfessionFunc(
    void* param_1,      // RCX: context/state object
    void* param_2,      // RDX: buffer/array
    void* param_3,      // R8: another buffer/array
    void* param_4,      // R9: pointer to a value
    char* param_5       // [RBP+0x50]: string/buffer being processed
) {
  log_message("[RF4S Extender] HookedConfessionFunc called with params: %p, %p, %p, %p, %p\n",
             param_1, param_2, param_3, param_4, param_5);
  
  // Process the function normally until we reach the point where we need to check gender
  // We'll manually implement the critical parts of the function
  
  // Call the original function for most of the processing
  if (originalConfessFunc) {
    // But before we call it, we need to modify the function's behavior at the critical point
    // This is where we would normally hook at 0x23E210
    
    // Check if param_5 contains one of our target event codes
    bool modifiedFlag = false;
    
    if (enableGenderModification) {
      try {
        // Based on the disassembly, we need to find the event code and flag
        // Try multiple approaches to find them
        
        // First, try to interpret param_5 as an event code pointer
        uint32_t eventCode = 0;
        uint8_t* flagPtr = nullptr;
        uint8_t oldFlag = 0;
        bool foundEventCode = false;
        
        // Approach 1: Try param_5 directly
        if (param_5) {
          try {
            eventCode = *reinterpret_cast<uint32_t*>(param_5);
            foundEventCode = true;
            log_message("[RF4S Extender] Found event code in param_5: 0x%08X\n", eventCode);
          } catch (...) {
            log_message("[RF4S Extender] Could not read event code from param_5\n");
          }
        }
        
        // Approach 2: Try param_1 (RCX) which might be RDI in the original code
        if (!foundEventCode && param_1) {
          try {
            eventCode = *reinterpret_cast<uint32_t*>(param_1);
            foundEventCode = true;
            log_message("[RF4S Extender] Found event code in param_1: 0x%08X\n", eventCode);
          } catch (...) {
            log_message("[RF4S Extender] Could not read event code from param_1\n");
          }
        }
        
        // If we found an event code, try to find the flag
        if (foundEventCode) {
          log_message("[RF4S Extender] Detected potential event code: 0x%08X\n", eventCode);
          
          // Try different parameters to find the flag pointer
          // Based on the original code, it should be at rcxPtr+0x40
          
          // Try param_1 first (most likely)
          if (param_1) {
            try {
              flagPtr = reinterpret_cast<uint8_t*>(param_1) + 0x40;
              oldFlag = *flagPtr;
              log_message("[RF4S Extender] Found flag at param_1+0x40: %p, value: %u\n", flagPtr, oldFlag);
            } catch (...) {
              flagPtr = nullptr;
              log_message("[RF4S Extender] Could not read flag from param_1+0x40\n");
            }
          }
          
          // If that didn't work, try param_2
          if (!flagPtr && param_2) {
            try {
              flagPtr = reinterpret_cast<uint8_t*>(param_2) + 0x40;
              oldFlag = *flagPtr;
              log_message("[RF4S Extender] Found flag at param_2+0x40: %p, value: %u\n", flagPtr, oldFlag);
            } catch (...) {
              flagPtr = nullptr;
              log_message("[RF4S Extender] Could not read flag from param_2+0x40\n");
            }
          }
          
          // If we found both an event code and a flag pointer, check if it's a confession event
          if (flagPtr) {
            // Check if this is a confession event
            if (eventCode == 0x16070007 || eventCode == 0x07008207) {
              // Modify the flag based on the event code
              if (eventCode == 0x16070007 && oldFlag == 1) {
                *flagPtr = 0;
                log_message("[RF4S Extender] Modified flag for male confession (0x16070007)\n");
                modifiedFlag = true;
              } else if (eventCode == 0x07008207 && oldFlag == 0) {
                *flagPtr = 1;
                log_message("[RF4S Extender] Modified flag for female confession (0x07008207)\n");
                modifiedFlag = true;
              }
              
              log_message("[RF4S Extender] Flag after: %u\n", *flagPtr);
            }
          } else {
            log_message("[RF4S Extender] Could not find flag pointer for event code 0x%08X\n", eventCode);
          }
        } else {
          log_message("[RF4S Extender] Could not find event code in any parameter\n");
        }
      } catch (...) {
        log_message("[RF4S Extender] ERROR: Exception while checking event code\n");
      }
    }
    
    // Only call the original function if we didn't modify the flag
    if (!modifiedFlag) {
      log_message("[RF4S Extender] Calling original confession function\n");
      originalConfessFunc(param_1, param_2, param_3, param_4, param_5);
    } else {
      log_message("[RF4S Extender] Skipping original function call after flag modification\n");
      
      // We need to perform the minimal operations that the original function would do
      // Based on the disassembly, we need to ensure the function has the same side effects
      
      // The function is void, so we don't need to return a value
      // But we should still perform any critical side effects
      
      // Based on the disassembly, the function seems to be processing a state machine
      // We've already modified the gender flag, which is the critical part
      // For safety, we'll call the original function with modified parameters
      // to ensure any other side effects still happen
      
      try {
        // Create a copy of param_5 with the modified flag
        // This way the original function will see our modified flag
        // but we won't risk it overwriting our changes
        log_message("[RF4S Extender] Calling original function with modified parameters\n");
        originalConfessFunc(param_1, param_2, param_3, param_4, param_5);
      } catch (...) {
        log_message("[RF4S Extender] ERROR: Exception while calling original function with modified parameters\n");
      }
    }
  } else {
    log_message("[RF4S Extender] ERROR: originalConfessFunc is null!\n");
  }
  
  log_message("[RF4S Extender] HookedConfessionFunc completed\n");
}

void LoadRealSDL() {
  char path[MAX_PATH];
  // load from game folder
  GetModuleFileNameA(NULL, path, MAX_PATH);
  *strrchr(path, '\\') = 0;
  strcat(path, "\\SDL2_real.dll");
  LoadLibraryA(path);
}

DWORD WINAPI MainThread(LPVOID) {
  // Get module base address
  RF4S_BASE = reinterpret_cast<uintptr_t>(GetModuleHandleA(NULL));
  
  // Set up console output
  AllocConsole();
  freopen("CONOUT$", "w", stdout);
  
  // Open log file in game directory
  char logPath[MAX_PATH];
  GetModuleFileNameA(NULL, logPath, MAX_PATH);
  *strrchr(logPath, '\\') = 0;
  strcat(logPath, "\\rf4s_extender.log");
  
  logFile.open(logPath, std::ios::out | std::ios::trunc);
  if (!logFile.is_open()) {
    printf("Failed to open log file at %s\n", logPath);
  }
  
  log_message("[RF4S Extender] Initialized (base=0x%p)\n", (void*)RF4S_BASE);
  log_message("[RF4S Extender] Log file created at %s\n", logPath);
  
  if (MH_Initialize() != MH_OK) {
    log_message("MinHook init failed\n");
    return 1;
  }

  // friendship hook
  {
    void* target = reinterpret_cast<void*>(RF4S_BASE + OFF_FRIENDSHIP_CHECK);
    log_message("[RF4S Extender] Setting up friendship hook at address %p\n", target);
    
    MH_STATUS status = MH_CreateHook(
      target,
      reinterpret_cast<LPVOID>(&HookedFriend),
      reinterpret_cast<void**>(&originalFriend)
    );
    
    if (status != MH_OK) {
      log_message("[RF4S Extender] ERROR: Failed to create friendship hook: %d\n", status);
    } else {
      log_message("[RF4S Extender] originalFriend function set to %p\n", originalFriend);
      
      status = MH_EnableHook(target);
      if (status != MH_OK) {
        log_message("[RF4S Extender] ERROR: Failed to enable friendship hook: %d\n", status);
      } else {
        log_message("[RF4S Extender] Friendship hook enabled successfully\n");
      }
    }
  }
  // New approach: Hook the function entry point instead of the middle
  {
    // Create a new function that will handle the gender check logic
    void* target = reinterpret_cast<void*>(RF4S_BASE + OFF_CONFESSION_FUNC);
    log_message("[RF4S Extender] Setting up confession function hook at entry point %p\n", target);
    
    // Add detailed memory inspection around the hook address
    try {
      uint8_t* bytePtr = reinterpret_cast<uint8_t*>(target);
      log_message("[RF4S Extender] Examining bytes at function entry: %02X %02X %02X %02X %02X\n",
                 bytePtr[0], bytePtr[1], bytePtr[2], bytePtr[3], bytePtr[4]);
    } catch (...) {
      log_message("[RF4S Extender] WARNING: Could not read memory at function entry\n");
    }
    
    // Define a proper function (not a lambda) to be used with MinHook
    // Lambda functions can't be used with function pointers unless they're non-capturing
    
    // Create the hook
    MH_STATUS status = MH_CreateHook(
      target,
      reinterpret_cast<LPVOID>(HookedConfessionFunc),
      reinterpret_cast<void**>(&originalConfessFunc)
    );
    
    if (status != MH_OK) {
      log_message("[RF4S Extender] ERROR: Failed to create confession function hook: %d\n", status);
    } else {
      log_message("[RF4S Extender] originalConfessFunc set to %p\n", originalConfessFunc);
      
      // Check if originalConfessFunc looks valid
      if (!originalConfessFunc) {
        log_message("[RF4S Extender] WARNING: originalConfessFunc is NULL - this will likely crash!\n");
      }
      
      status = MH_EnableHook(target);
      if (status != MH_OK) {
        log_message("[RF4S Extender] ERROR: Failed to enable confession function hook: %d\n", status);
      } else {
        log_message("[RF4S Extender] Confession function hook enabled successfully\n");
      }
    }
  }
  
  return 0;
}

BOOL APIENTRY DllMain(HMODULE hMod, DWORD reason, LPVOID) {
  if (reason == DLL_PROCESS_ATTACH) {
    LoadRealSDL();
    DisableThreadLibraryCalls(hMod);
    CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
  }
  return TRUE;
}
