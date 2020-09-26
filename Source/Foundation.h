#pragma once

#ifdef UNICODE
    #define _tstring std::wstring
#else
    #define _tstring std::string
#endif

template<class T>
static inline void ForceWrite(T& dst, const T& src)
{
    DWORD old_flag;
    ::VirtualProtect(&dst, sizeof(T), PAGE_EXECUTE_READWRITE, &old_flag);
    dst = src;
    ::VirtualProtect(&dst, sizeof(T), old_flag, &old_flag);
}

bool IsValidMemory(const void* p);
bool IsValidModule(HMODULE module);

HMODULE GetModuleByAddr(const void* p);
std::string GetModuleDirectoryA(HMODULE module);
std::wstring GetModuleDirectoryW(HMODULE module);
std::string GetCurrentModuleDirectoryA();
std::wstring GetCurrentModuleDirectoryW();
std::string GetMainModulePathA();
std::wstring GetMainModulePathW();
std::string GetMainModuleDirectoryA();
std::wstring GetMainModuleDirectoryW();
std::string GetMainModuleFilenameA();
std::wstring GetMainModuleFilenameW();

std::string GetMainModulePathA(HANDLE proc);
std::wstring GetMainModulePathW(HANDLE proc);
std::string GetMainModuleDirectoryA(HANDLE proc);
std::wstring GetMainModuleDirectoryW(HANDLE proc);
bool IsModuleLoadedA(DWORD pid, const char* dllname);
bool IsModuleLoadedW(DWORD pid, const wchar_t* dllname);


#ifdef UNICODE

#define _tstring std::wstring
#define GetModuleDirectory          GetModuleDirectoryW
#define GetCurrentModuleDirectory   GetCurrentModuleDirectoryW
#define GetMainModulePath           GetMainModulePathW
#define GetMainModuleDirectory      GetMainModuleDirectoryW
#define GetMainModuleFilename       GetMainModuleFilenameW
#define GetMainModulePath           GetMainModulePathW
#define GetMainModuleDirectory      GetMainModuleDirectoryW
#define IsModuleLoaded              IsModuleLoadedW

#else

#define _tstring std::string
#define GetModuleDirectory          GetModuleDirectoryA
#define GetCurrentModuleDirectory   GetCurrentModuleDirectoryA
#define GetMainModulePath           GetMainModulePathA
#define GetMainModuleDirectory      GetMainModuleDirectoryA
#define GetMainModuleFilename       GetMainModuleFilenameA
#define GetMainModulePath           GetMainModulePathA
#define GetMainModuleDirectory      GetMainModuleDirectoryA
#define IsModuleLoaded              IsModuleLoadedA

#endif


void* AllocExecutable(size_t size, void* location);
void* EmitJmpInstruction(void* from_, void* to_);
void* OverrideEAT(HMODULE module, const char* funcname, void* replacement, void*& jump_table);
void* OverrideIAT(HMODULE module, const char* target_module, const char* funcname, void* replacement);
void OverrideIAT(const char* target_module, const char* funcname, void* replacement);

void EnumerateModules(HANDLE process, const std::function<void(HMODULE)>& body);
void EnumerateModules(const std::function<void(HMODULE)>& body);
void EnumerateDLLImports(HMODULE module, const char* dllname, const std::function<void(const char*, void*&)>& body);
void EnumerateDLLExports(HMODULE module, const std::function<void(const char*, void*&)>& body);

DWORD FindProcessID(const TCHAR* exe);
// caller must call CloseHandle() for returned handle
HANDLE FindProcess(const TCHAR* exe);

void EnumerateThreads(DWORD pid, const std::function<void(DWORD)>& proc);
void EnumerateThreads(const std::function<void(DWORD)>& proc);
DWORD GetMainThreadID();
bool IsInMainThread();
