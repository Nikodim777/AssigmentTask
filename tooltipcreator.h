#ifndef TOOLTIPCREATOR_H_INCLUDED
#define TOOLTIPCREATOR_H_INCLUDED

#define _WIN32_WINNT 0x0501
#define _WIN32_IE  0x0500

#include "windows.h"
#include <commctrl.h>

HWND CreateToolTip(HWND hwnd, HINSTANCE hinst);
void AddToolForTip(HWND toolTip, HWND tool, HINSTANCE hinst, LONG left, LONG top, LONG right, LONG bottom, LPTSTR tip, UINT flags);

#endif // TOOLTIPCREATOR_H_INCLUDED
