#include "pch.h"
#include "Foundation.h"
#include "TimeDistorter.h"

static double g_time_rate = 1.0f;



// QueryPerformanceCounter

using QueryPerformanceCounter_t = BOOL (*)(LARGE_INTEGER* lpPerformanceCount);
QueryPerformanceCounter_t QueryPerformanceCounter_orig;

static BOOL QueryPerformanceCounter_hook(LARGE_INTEGER* lpPerformanceCount)
{
    static LONGLONG s_start, s_prev;
    static double s_progress;

    auto ret = QueryPerformanceCounter_orig(lpPerformanceCount);
    if (lpPerformanceCount) {
        if (s_start == 0)
            s_start = s_prev = lpPerformanceCount->QuadPart;

        auto diff = lpPerformanceCount->QuadPart - s_prev;
        s_progress += double(diff) * g_time_rate;
        s_prev = lpPerformanceCount->QuadPart;

        auto time_scaled = s_start + (LONGLONG)s_progress;
        lpPerformanceCount->QuadPart = time_scaled;
    }
    return ret;
}


// GetSystemTimePreciseAsFileTime
using GetSystemTimePreciseAsFileTime_t = void (*)(LPFILETIME lpSystemTimeAsFileTime);
GetSystemTimePreciseAsFileTime_t GetSystemTimePreciseAsFileTime_orig;

static void GetSystemTimePreciseAsFileTime_hook(LPFILETIME lpSystemTimeAsFileTime)
{
    GetSystemTimePreciseAsFileTime_orig(lpSystemTimeAsFileTime);
}


// DWORD timeGetTime();
using timeGetTime_t = DWORD (*)();
timeGetTime_t timeGetTime_orig;

static DWORD timeGetTime_hook()
{
    static DWORD s_start, s_prev;
    static double s_progress;

    auto ret = timeGetTime_orig();
    if (s_start == 0)
        s_start = s_prev = ret;

    auto diff = ret - s_prev;
    s_progress += double(diff) * g_time_rate;
    s_prev = ret;

    auto time_scaled = s_start + (DWORD)s_progress;
    return time_scaled;
}



tdAPI void tdSetTimeRate(double v)
{
    g_time_rate = v;
}

tdAPI void tdSetHooks()
{
    {
        auto mod = ::GetModuleHandleA("kernel32.dll");
        if (mod) {
            (void*&)QueryPerformanceCounter_orig = ::GetProcAddress(mod, "QueryPerformanceCounter");
            OverrideIAT("kernel32.dll", "QueryPerformanceCounter", QueryPerformanceCounter_hook);

            (void*&)GetSystemTimePreciseAsFileTime_orig = ::GetProcAddress(mod, "GetSystemTimePreciseAsFileTime");
            OverrideIAT("kernel32.dll", "GetSystemTimePreciseAsFileTime", GetSystemTimePreciseAsFileTime_hook);
        }
    }
    {
        auto mod = ::GetModuleHandleA("winmm.dll");
        if (mod) {
            (void*&)timeGetTime_orig = ::GetProcAddress(mod, "timeGetTime");
            OverrideIAT("winmm.dll", "timeGetTime", timeGetTime_hook);
        }
    }
}
