#include "pch.h"
#include "TimeDistorter.h"

void tdSetHooks();

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH) {
        tdSetHooks();
    }
    else if (fdwReason == DLL_PROCESS_DETACH) {
    }
    return TRUE;
}
