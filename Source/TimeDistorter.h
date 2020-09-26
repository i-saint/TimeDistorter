#pragma once

#define tdAPI extern "C" __declspec(dllexport)

tdAPI void tdSetTimeScale(double v);
tdAPI double tdGetTimeScale();
tdAPI void tdSetHooks();

tdAPI bool tdOpenTimeWindow();
tdAPI bool tdCloseTimeWindow();
tdAPI void tdTimeWindowLoop();
