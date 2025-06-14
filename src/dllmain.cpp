#include <windows.h>
#include "core/scriptloader.h"
#include "core/logger.h"
#include "mods/everyone_is_bi/bi_mod.h"

// ——— SDL2 loading ———
void LoadRealSDL() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    *strrchr(path, '\\') = 0;
    strcat(path, "\\SDL2_real.dll");
    LoadLibraryA(path);
}

// ——— Main initialization thread ———
DWORD WINAPI MainThread(LPVOID) {
    // Initialize the core ScriptLoader system
    RF4S::ScriptLoader& scriptLoader = RF4S::ScriptLoader::GetInstance();
    
    if (!scriptLoader.Initialize()) {
        // If we can't initialize the core system, there's not much we can do
        // The logger might not be available, so we'll try basic output
        printf("[RF4S ScriptLoader] FATAL: Failed to initialize core system\n");
        return 1;
    }
    
    // Initialize the built-in "Everyone is Bi" mod
    // In the future, this will be replaced by a proper mod loading system
    RF4S::EveryoneIsBiMod& biMod = RF4S::EveryoneIsBiMod::GetInstance();
    if (!biMod.Initialize()) {
        RF4S_LOG_ERROR("Failed to initialize Everyone is Bi mod");
        // Continue anyway - the core system is still functional
    }
    
    RF4S_LOG_INFO("RF4S ScriptLoader startup complete");
    return 0;
}

// ——— DLL entry point ———
BOOL APIENTRY DllMain(HMODULE hMod, DWORD reason, LPVOID) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            // Load the real SDL2 library first
            LoadRealSDL();
            
            // Disable thread library calls for performance
            DisableThreadLibraryCalls(hMod);
            
            // Start initialization in a separate thread
            CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
            break;
            
        case DLL_PROCESS_DETACH:
            // Shutdown the ScriptLoader system
            RF4S::ScriptLoader::GetInstance().Shutdown();
            break;
    }
    
    return TRUE;
}
