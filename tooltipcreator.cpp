#include "tooltipcreator.h"

HWND CreateToolTip(HWND hwnd, HINSTANCE hinst)
{
    return CreateWindow(TOOLTIPS_CLASS, NULL, TTS_ALWAYSTIP | TTS_BALLOON,
                        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                        hwnd, NULL, hinst, NULL);
}

void AddToolForTip(HWND toolTip, HWND tool, HINSTANCE hinst, LONG left, LONG top, LONG right, LONG bottom, LPTSTR tip, UINT flags)
{
    TOOLINFO tipInfo = {0};
    tipInfo.cbSize = sizeof(TOOLINFO);
    if(flags & TTF_IDISHWND) {
        tipInfo.uId = (UINT)tool;
    } else {
        tipInfo.hwnd = tool;
    }
    tipInfo.hinst = hinst;
    tipInfo.uFlags = flags;
    tipInfo.lpszText = tip;
    tipInfo.rect = {left, top, right, bottom};

    SendMessage(toolTip, TTM_ADDTOOL, 0, (LPARAM)&tipInfo);
}
