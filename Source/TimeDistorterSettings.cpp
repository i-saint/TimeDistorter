#include "pch.h"
#include "Foundation.h"
#include "TimeDistorter.h"

#define tdSharedBufferSize 256
#define tdSharedMemNameFormat "Local\\TimeDistorter%d"

static double g_time_scale = 1.0f;
static char g_shared_mem_name[256];
static HANDLE g_shared_memory;

struct tdSettings
{
    double time_scale = 1.0;
};
static tdSettings g_td_settings;


tdAPI bool tdCreateSettings(int pid)
{
    sprintf(g_shared_mem_name, tdSharedMemNameFormat, pid);

    g_shared_memory = ::CreateFileMappingA(
        INVALID_HANDLE_VALUE,   // use paging file
        NULL,                   // default security
        PAGE_EXECUTE_READWRITE, // read/write access
        0,                      // maximum object size (high-order DWORD)
        tdSharedBufferSize,     // maximum object size (low-order DWORD)
        g_shared_mem_name);     // name of mapping object

    if (!g_shared_memory) {
        PSTR buf = nullptr;
        DWORD reason = GetLastError();
        size_t size = ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, reason, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buf, 0, NULL);

        std::string message(buf, size);
        printf("Could not create file mapping object (%s).\n", message.c_str());
    }

    return g_shared_memory != nullptr;
}

tdAPI bool tdOpenSettings(int pid)
{
    sprintf(g_shared_mem_name, tdSharedMemNameFormat, pid);

    g_shared_memory = ::OpenFileMappingA(
        PAGE_READONLY,          // read/write access
        FALSE,                  // do not inherit the name
        g_shared_mem_name);     // name of mapping object

    return g_shared_memory != nullptr;
}

tdAPI void tdCloseSettings()
{
    if (g_shared_memory) {
        ::CloseHandle(g_shared_memory);
        g_shared_memory = nullptr;
    }
}

tdAPI void tdSetTimeScale(double v)
{
    g_td_settings.time_scale = v;

    if (g_shared_memory) {
        auto* dst = (tdSettings*)::MapViewOfFile(g_shared_memory, FILE_MAP_WRITE, 0, 0, tdSharedBufferSize);
        if (dst) {
            *dst = g_td_settings;
            ::UnmapViewOfFile(dst);
        }
    }
}

tdAPI double tdGetTimeScale()
{
    if (g_shared_memory) {
        auto* src = (const tdSettings*)::MapViewOfFile(g_shared_memory, FILE_MAP_READ, 0, 0, tdSharedBufferSize);
        if (src) {
            g_td_settings = *src;
            ::UnmapViewOfFile(src);
        }
    }
    return g_td_settings.time_scale;
}

