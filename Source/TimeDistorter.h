#pragma once

#define tdAPI extern "C" __declspec(dllexport)

tdAPI void tdSetTimeScale(double v);
tdAPI double tdGetTimeScale();
