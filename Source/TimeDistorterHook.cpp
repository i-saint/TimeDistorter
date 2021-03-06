﻿#include "pch.h"
#include "Foundation.h"
#include "TimeDistorter.h"




// QueryPerformanceCounter

using QueryPerformanceCounter_t = BOOL (*)(LARGE_INTEGER* lpPerformanceCount);
QueryPerformanceCounter_t QueryPerformanceCounter_orig;

static BOOL QueryPerformanceCounter_hook(LARGE_INTEGER* lpPerformanceCount)
{
    thread_local LONGLONG s_start, s_prev;
    thread_local double s_progress;

    auto ret = QueryPerformanceCounter_orig(lpPerformanceCount);
    if (lpPerformanceCount) {
        if (s_start == 0)
            s_start = s_prev = lpPerformanceCount->QuadPart;

        auto diff = lpPerformanceCount->QuadPart - s_prev;
        s_progress += double(diff) * tdGetTimeScale();
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
    // todo
    GetSystemTimePreciseAsFileTime_orig(lpSystemTimeAsFileTime);
}


// DWORD timeGetTime();
using timeGetTime_t = DWORD (*)();
timeGetTime_t timeGetTime_orig;

static DWORD timeGetTime_hook()
{
    thread_local DWORD s_start, s_prev;
    thread_local double s_progress;

    auto ret = timeGetTime_orig();
    if (s_start == 0)
        s_start = s_prev = ret;

    auto diff = ret - s_prev;
    s_progress += double(diff) * tdGetTimeScale();
    s_prev = ret;

    auto time_scaled = s_start + (DWORD)s_progress;
    return time_scaled;
}



void tdSetHooks()
{
    auto exe = GetMainModuleFilenameA();
    if (strstr(exe.c_str(), "TimeDistorter")) {
        // ignore if host is injector
        return;
    }

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
