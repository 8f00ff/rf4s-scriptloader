#include <windows.h>
#include <cstdio>

HMODULE realSDL = NULL;

void LoadRealSDL() {
  char path[MAX_PATH];
  GetSystemDirectoryA(path, MAX_PATH);
  strcat(path, "\\SDL2_real.dll");
  realSDL = LoadLibraryA(path);
}

DWORD WINAPI MainThread(LPVOID) {
  AllocConsole();
  freopen("CONOUT$", "w", stdout);
  SetConsoleTitleA("RF4S Extender Console");
  printf("[RF4S Extender] Proxy SDL2 loaded!\n");
  
  // TODO: hooks go here
  
  return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
  if (reason == DLL_PROCESS_ATTACH) {
    LoadRealSDL();
    CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
  }
  return TRUE;
}
