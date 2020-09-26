#pragma once

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <ctime>
#include <cassert>
#include <cstdint>

#include <windows.h>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <commctrl.h>
