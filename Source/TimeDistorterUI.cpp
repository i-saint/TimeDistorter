#include "pch.h"
#include "TimeDistorter.h"
#include "resource.h"

HWND g_td_time_window;
HWND g_td_time_slider;
HWND g_td_current_window;

static bool CtrlIsChecked(int cid)
{
    return ::IsDlgButtonChecked(g_td_current_window, cid);
};

static void CtrlSetCheck(int cid, bool v)
{
    ::CheckDlgButton(g_td_current_window, cid, v);
};

static int CtrlGetInt(int cid, int default_value, bool sign = true)
{
    BOOL valid;
    int v = ::GetDlgItemInt(g_td_current_window, cid, &valid, sign);
    return valid ? v : default_value;
};


static INT_PTR CALLBACK tdTimeWindowCB(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    g_td_current_window = hDlg;

    INT_PTR ret = FALSE;
    switch (msg) {
    case WM_INITDIALOG:
    {
        g_td_time_slider = ::GetDlgItem(hDlg, IDC_SLIDER_TIME);
        ::SendMessageA(g_td_time_slider, TBM_SETRANGEMIN, 0, 0);
        ::SendMessageA(g_td_time_slider, TBM_SETRANGEMAX, 0, 100);
        ::SendMessageA(g_td_time_slider, TBM_SETPAGESIZE, 0, 5);
        ::SendMessageA(g_td_time_slider, TBM_SETPOS, TRUE, 10);

        ret = TRUE;
        break;
    }

    case WM_CLOSE:
        tdSetTimeScale(1.0);
        ::DestroyWindow(hDlg);
        break;

    case WM_DESTROY:
        g_td_time_window = nullptr;
        break;

    case WM_COMMAND:
    {
        int code = HIWORD(wParam);
        int cid = LOWORD(wParam);

        switch (cid) {
        case IDOK:
            ::DestroyWindow(hDlg);
            break;
        case IDCANCEL:
            ::DestroyWindow(hDlg);
            break;
        default:
            break;
        }
        break;
    }

    case WM_HSCROLL:
    {
        double pos = (double)::SendMessageA(g_td_time_slider, TBM_GETPOS, 0, 0) / 10.0;
        tdSetTimeScale(pos);
        break;
    }

    default:
        break;
    }
    return ret;
}

tdAPI bool tdOpenTimeWindow()
{
    if (!g_td_time_window) {
        g_td_time_window = ::CreateDialogParam(::GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_TIME_WINDOW), nullptr, tdTimeWindowCB, (LPARAM)nullptr);
        ::ShowWindow(g_td_time_window, SW_SHOW);
        ::UpdateWindow(g_td_time_window);
        return true;
    }
    return false;
}

tdAPI bool tdCloseTimeWindow()
{
    if (g_td_time_window) {
        ::PostMessage(g_td_time_window, WM_CLOSE, 0, 0);
        return true;
    }
    return false;
}

tdAPI void tdTimeWindowLoop()
{
    MSG msg;
    while (g_td_time_window && ::GetMessage(&msg, g_td_time_window, 0, 0)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}

