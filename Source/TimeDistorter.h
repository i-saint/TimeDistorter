#pragma once

#define tdAPI extern "C" __declspec(dllexport)

tdAPI bool tdCreateSettings(int pid);
tdAPI bool tdOpenSettings(int pid);
tdAPI void tdCloseSettings();
tdAPI void tdSetTimeScale(double v);
tdAPI double tdGetTimeScale();
tdAPI void tdSetHooks();

tdAPI bool tdOpenTimeWindow();
tdAPI bool tdCloseTimeWindow();
tdAPI void tdTimeWindowLoop();
