#define _WIN32_WINNT 0x0501

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "Example.h"
#include "MaxiMinAlg.h"
#include "StepInfo.h"
#include "resources.h"
#include "manageToolWnd.h"
#include "hoverableWindow.h"
#include "tooltipcreator.h"

extern HINSTANCE hinst;

static HWND btnPrev, btnNext, btnPrevTip, btnNextTip;
static HDC paintDC;
static PAINTSTRUCT paintStruct;
static RECT nameStepRect = {60, 20, 290, 40}, descryptStepRect = {10, 60, 340, 340};
static TCHAR nameStep[64] = _T("Заголовок шага"), descryptStep[2048] = _T("Длинное-предлинное описание текущего шага.");
static HRGN circleRgnP, circleRgnN, triRgnP, triRgnN, rndTriRgnP, rndTriRgnN, rndTriRgnDrawP, rndTriRgnDrawN;
static POINT triangleP[3] = {{-7, 20}, {30, -3}, {30, 41}},
             triangleN[3] = {{47, 20}, {10, -3}, {10, 41}};
static LPDRAWITEMSTRUCT drawStruct;
static HBRUSH btnBrush, borderBtnBrush;

static HFONT mainFont;

static mouseContext mContBtnPrev, mContBtnNext;

static AssignTaskAlgorithm *solver;
static StepInfo *info;
static int numStep;

LRESULT CALLBACK ManageToolWndProc(HWND hwnd, UINT msg, WPARAM wpar, LPARAM lpar)
{
    switch(msg)
    {
        case WM_CREATE:
            btnBrush = CreateSolidBrush(colors[BUTTCOLOR]);
            borderBtnBrush = CreateSolidBrush(colors[BORDERBUTTCOLOR]);

            mainFont = CreateFont(28, SEGOE_SCRIPT_FONT);

            btnPrev = CreateWindow(_T("BUTTON"), _T(""), WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | BS_OWNERDRAW,
                                    10, 10, 40, 40, hwnd, NULL, hinst, NULL);
            btnNext = CreateWindow(_T("BUTTON"), _T(""), WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | BS_OWNERDRAW,
                                    300, 10, 40, 40, hwnd, NULL, hinst, NULL);

            circleRgnP = CreateEllipticRgn(0, 0, 40, 40);
            triRgnP = CreatePolygonRgn(triangleP, 3, WINDING);
            rndTriRgnP = CreateRectRgn(0, 0, 0, 0);
            IntersectRgn(rndTriRgnP, circleRgnP, triRgnP);

            rndTriRgnDrawP = CreateRectRgn(0, 0, 0, 0);
            CopyRgn(rndTriRgnDrawP, rndTriRgnP);

            SetWindowRgn(btnPrev, rndTriRgnP, TRUE);

            circleRgnN = CreateEllipticRgn(0, 0, 40, 40);
            triRgnN = CreatePolygonRgn(triangleN, 3, WINDING);
            rndTriRgnN = CreateRectRgn(0, 0, 0, 0);
            IntersectRgn(rndTriRgnN, circleRgnN, triRgnN);

            rndTriRgnDrawN = CreateRectRgn(0, 0, 0, 0);
            CopyRgn(rndTriRgnDrawN, rndTriRgnN);

            SetWindowRgn(btnNext, rndTriRgnN, TRUE);

            btnPrevTip = CreateToolTip(hwnd, hinst);
            btnNextTip = CreateToolTip(hwnd, hinst);

            AddToolForTip(btnPrevTip, btnPrev, hinst, 0, 0, 0, 0, (LPTSTR)_T("Предыдущий шаг"), TTF_IDISHWND);
            AddToolForTip(btnNextTip, btnNext, hinst, 0, 0, 0, 0, (LPTSTR)_T("Следующий шаг"), TTF_IDISHWND);

            mContBtnPrev.track.cbSize = mContBtnNext.track.cbSize = sizeof(TRACKMOUSEEVENT);
            mContBtnPrev.track.dwFlags = mContBtnNext.track.dwFlags = TME_LEAVE;
            mContBtnPrev.track.dwHoverTime = mContBtnNext.track.dwHoverTime = HOVER_DEFAULT;
            mContBtnPrev.track.hwndTrack = btnPrev;
            mContBtnNext.track.hwndTrack = btnNext;

            mContBtnPrev.isOver = mContBtnNext.isOver = FALSE;
            mContBtnPrev.windowTip = btnPrevTip;
            mContBtnNext.windowTip = btnNextTip;

            SetWindowSubclass(btnPrev, HoverableWindowProc, 0, (DWORD_PTR)&mContBtnPrev);
            SetWindowSubclass(btnNext, HoverableWindowProc, 0, (DWORD_PTR)&mContBtnNext);

            solver = new MaxiMinAlg();
            numStep = 0;
            return 0;

        case WM_SIZE:
            if((UINT)wpar == SIZE_MINIMIZED)
                SendMessage(GetParent(hwnd), WM_CHECK_MENU_ITEM, (WPARAM)hwnd, (LPARAM)MF_UNCHECKED);
            else if((UINT)wpar == SIZE_RESTORED)
                SendMessage(GetParent(hwnd), WM_CHECK_MENU_ITEM, (WPARAM)hwnd, (LPARAM)MF_CHECKED);
            return 0;

        case WM_PAINT:
            paintDC = BeginPaint(hwnd, &paintStruct);
            SetBkMode(paintDC, TRANSPARENT);
            SelectFont(paintDC, mainFont);

            DrawText(paintDC, nameStep, _tcslen(nameStep), &nameStepRect, DT_END_ELLIPSIS | DT_SINGLELINE | DT_CENTER);
            DrawText(paintDC, descryptStep, _tcslen(descryptStep), &descryptStepRect, DT_WORDBREAK | DT_CENTER | DT_END_ELLIPSIS);

            EndPaint(hwnd, &paintStruct);
            return 0;

        case WM_COMMAND:
            if(HIWORD(wpar) == BN_CLICKED)
            {
                if(lpar == (LPARAM)btnPrev)
                    numStep--;
                else if(lpar == (LPARAM)btnNext )
                    numStep++;

                info = solver->GetStepInfo(&numStep);
                if(info != NULL)
                {
                    _tcscpy(nameStep, info->nameStep);
                    _tcscpy(descryptStep, info->descryptStep);
                    InvalidateRect(hwnd, NULL, true);
                }
            }
            return 0;

        case WM_LBUTTONDBLCLK:
            ShowWindow(hwnd, SW_MINIMIZE);
            SendMessage(GetParent(hwnd), WM_REDRAW_AUX_WINDOW, (WPARAM)hwnd, 0);
            return 0;

        case WM_DRAWITEM:
            drawStruct = (LPDRAWITEMSTRUCT) lpar;
            if(drawStruct->hwndItem == btnPrev)
            {
                if(drawStruct->itemState & ODS_SELECTED)
                    FillRgn(drawStruct->hDC, rndTriRgnDrawP, borderBtnBrush);
                else
                    FillRgn(drawStruct->hDC, rndTriRgnDrawP, btnBrush);

                if(mContBtnPrev.isOver && !(drawStruct->itemState & ODS_SELECTED))
                    FrameRgn(drawStruct->hDC, rndTriRgnDrawP, btnBrush, 2, 2);
                else
                    FrameRgn(drawStruct->hDC, rndTriRgnDrawP, borderBtnBrush, 2, 2);
            }
            else if(drawStruct->hwndItem == btnNext)
            {
                if(drawStruct->itemState & ODS_SELECTED)
                    FillRgn(drawStruct->hDC, rndTriRgnDrawN, borderBtnBrush);
                else
                    FillRgn(drawStruct->hDC, rndTriRgnDrawN, btnBrush);

                if(mContBtnNext.isOver && !(drawStruct->itemState & ODS_SELECTED))
                    FrameRgn(drawStruct->hDC, rndTriRgnDrawN, btnBrush, 2, 2);
                else
                    FrameRgn(drawStruct->hDC, rndTriRgnDrawN, borderBtnBrush, 2, 2);
            }
            return 0;

        case WM_PRESOLVE:
            solver->Solve((Example*)wpar);

            info = solver->GetStepInfo(&numStep);
            if(info != NULL)
            {
                _tcscpy(nameStep, info->nameStep);
                _tcscpy(descryptStep, info->descryptStep);
                InvalidateRect(hwnd, NULL, true);
            }
            return 0;

        case WM_CLEAR_SOLUTION:
            solver->ClearSolution();
            numStep = 0;
            return 0;

        case WM_DESTROY:
            DeleteRgn(circleRgnP);
            DeleteRgn(circleRgnN);
            DeleteRgn(triRgnP);
            DeleteRgn(triRgnN);
            DeleteRgn(rndTriRgnP);
            DeleteRgn(rndTriRgnN);
            DeleteRgn(rndTriRgnDrawP);
            DeleteRgn(rndTriRgnDrawN);

            DeleteBrush(btnBrush);
            DeleteBrush(borderBtnBrush);

            DeleteFont(mainFont);

            RemoveWindowSubclass(btnPrev, HoverableWindowProc, 0);
            RemoveWindowSubclass(btnNext, HoverableWindowProc, 0);

            delete solver;
            return 0;

        default:
            return DefWindowProc(hwnd, msg, wpar, lpar);
    }
}
