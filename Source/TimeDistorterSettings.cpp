#include "pch.h"
#include "Foundation.h"
#include "TimeDistorter.h"


struct tdSettings
{
    double time_scale = 1.0;
};

#pragma comment(linker, "/SECTION:.shared,RWS")
#pragma data_seg(".shared")
tdSettings g_td_settings;
#pragma data_seg()


tdAPI void tdSetTimeScale(double v)
{
    g_td_settings.time_scale = v;
}

tdAPI double tdGetTimeScale()
{
    return g_td_settings.time_scale;
}

