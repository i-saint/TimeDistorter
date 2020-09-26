#include "pch.h"
#include "Foundation.h"
#include "TimeDistorter.h"

#if defined(_M_IX86)
    #define tdCoreDLL "TimeDistorterDLL32.dll"
#elif defined(_M_X64)
    #define tdCoreDLL "TimeDistorterDLL64.dll"
#endif

struct ctxFindWindowByText
{
    HWND result = nullptr;
    const TCHAR* text = nullptr;
};

static BOOL CALLBACK cbFindWindowByText(HWND hwnd, LPARAM lParam)
{
    auto* ctx = (ctxFindWindowByText*)lParam;

    wchar_t title[4096];
    ::GetWindowTextW(hwnd, title, _countof(title));
    if (_tcsstr(title, ctx->text)) {
        ctx->result = hwnd;
    }
    return TRUE;
}

static HWND FindWindowByText(const TCHAR* name)
{
    ctxFindWindowByText ctx;
    ctx.text = name;
    ::EnumWindows(&cbFindWindowByText, (LPARAM)&ctx);
    return ctx.result;
}

static DWORD FindProcessByWindowText(const TCHAR* name)
{
    HWND hwnd = FindWindowByText(name);
    DWORD pid = 0;
    ::GetWindowThreadProcessId(hwnd, &pid);
    return pid;
}



static bool InjectDLL(HANDLE hProcess, const char* dllname)
{
    SIZE_T bytesRet = 0;
    DWORD oldProtect = 0;
    LPVOID remote_addr = NULL;
    HANDLE hThread = NULL;
    size_t len = strlen(dllname) + 1;

    remote_addr = ::VirtualAllocEx(hProcess, 0, 1024, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (remote_addr == NULL) { return false; }
    ::VirtualProtectEx(hProcess, remote_addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
    ::WriteProcessMemory(hProcess, remote_addr, dllname, len, &bytesRet);
    ::VirtualProtectEx(hProcess, remote_addr, len, oldProtect, &oldProtect);

    hThread = ::CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)((void*)&LoadLibraryA), remote_addr, 0, NULL);
    ::WaitForSingleObject(hThread, 3000);
    ::VirtualFreeEx(hProcess, remote_addr, 0, MEM_RELEASE);
    return true;
}

static bool InjectDLL(DWORD pid, const char* dllname)
{
    bool result = false;
    HANDLE process = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (process != INVALID_HANDLE_VALUE) {
        result = InjectDLL(process, dllname);
        ::CloseHandle(process);
    }
    return result;
}


int wmain(int argc, TCHAR *argv[])
{
    if (argc < 2) {
        _tprintf(_T("usage %s /target:target_aplication.exe /patch:c:\\fulllpath\\to\\patch.dll\n"), GetMainModuleFilename().c_str());
        return 0;
    }

    DWORD target_pid = 0;
    float speed = -1;

    for (int i = 1; i < argc; ++i) {
        if (_tcsncmp(argv[i], _T("/target:"), 8)==0) {
            const auto* name = argv[i] + 8;
            target_pid = FindProcessID(name);
        }
        else if (_tcsncmp(argv[i], _T("/targetwindow:"), 14) == 0) {
            const auto* window = argv[i] + 14;
            target_pid = FindProcessByWindowText(window);
        }
        else if (_tcsncmp(argv[i], _T("/targetpid:"), 11) == 0) {
            const auto* pid = argv[i] + 11;
            target_pid = _wtoi(pid);
        }
        else if (_tcsncmp(argv[i], _T("/speed:"), 7) == 0) {
            speed = (float)_wtof(argv[i] + 7);
        }
    }

    if (target_pid != 0) {
        if (IsModuleLoaded(target_pid, _T(tdCoreDLL))) {
            // already loaded. just open settings dialog.
            tdOpenTimeWindow();
            tdTimeWindowLoop();
            tdCloseSettings();
        }
        else {
            // create shared memory for settings
            tdCreateSettings(target_pid);

            auto coredllpath = std::string(GetMainModuleDirectoryA()) + "\\" + tdCoreDLL;
            if (InjectDLL(target_pid, coredllpath.c_str())) {
                tdOpenTimeWindow();
                tdTimeWindowLoop();
            }
            else {
                printf("injection failed\n");
                return 1;
            }

            tdCloseSettings();
        }
    }


    return 0;
}
