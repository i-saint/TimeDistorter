#pragma once

#define tdAPI extern "C" __declspec(dllexport)

tdAPI void tdSetTimeRate(double v);
tdAPI void tdSetHooks();
tdAPI void tdOpenTimeWindow();
