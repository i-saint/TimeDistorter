#include "pch.h"
#include "TimeDistorter.h"


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH) {
        tdOpenSettings(::GetCurrentProcessId());
        tdSetHooks();
    }
    else if (fdwReason == DLL_PROCESS_DETACH) {
        tdCloseSettings();
    }
    return TRUE;
}
