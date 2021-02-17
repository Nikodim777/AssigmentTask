#define _WIN32_WINNT 0x0501

#include <stdio.h>
#include <time.h>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "hoverableWindow.h"

static MSG tempMsg;

//Добавляет обработку наведения мыши на окно
LRESULT CALLBACK HoverableWindowProc(HWND hwnd, UINT msg, WPARAM wpar, LPARAM lpar, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    switch(msg)
    {
        case WM_MOUSEMOVE:
            if(!((mouseContext*)dwRefData)->isOver)
            {
                ((mouseContext*)dwRefData)->isOver = TRUE;
                TrackMouseEvent(&((mouseContext*)dwRefData)->track);
                InvalidateRect(hwnd, NULL, FALSE);
            }

            tempMsg.hwnd = hwnd;
            tempMsg.message = msg;
            tempMsg.lParam = lpar;
            tempMsg.wParam = wpar;
            SendMessage(((mouseContext*)dwRefData)->windowTip, TTM_RELAYEVENT, 0, (LPARAM)(LPMSG)&tempMsg);
            return 0;

        case WM_MOUSELEAVE:
            ((mouseContext*)dwRefData)->isOver = FALSE;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;

        default:
            return DefSubclassProc(hwnd, msg, wpar, lpar);
    }
}
