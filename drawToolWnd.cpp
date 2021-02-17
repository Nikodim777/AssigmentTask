#define _WIN32_WINNT 0x0501
#define _WIN32_IE  0x0500

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include "resources.h"
#include "vertexdlg.h"
#include "drawToolWnd.h"
#include "hoverableWindow.h"
#include "tooltipcreator.h"

class Drawer;

extern HINSTANCE hinst;

static HWND btnVertex, btnLabel, btnColor, btnVertexTip, btnLabelTip,
       labelColorTip, titleColorTip, vertexColorTip, linkColorTip;
static LPDRAWITEMSTRUCT drawStruct;
static HBRUSH btnBrush, borderBtnBrush, labelColorBrush, titleColorBrush, linkColorBrush, vertexColorBrush;
static HPEN btnPen, borderBtnPen, labelColorPen, vertexColorPen;

static CHOOSECOLOR chsclr = {0};

static mouseContext mContBtnLabel, mContBtnVertex;

static INITCOMMONCONTROLSEX initcc = {0};

static Drawer* drawer;

LRESULT CALLBACK DrawToolWndProc(HWND hwnd, UINT msg, WPARAM wpar, LPARAM lpar)
{
    switch(msg) {
        case WM_CREATE:
            initcc.dwSize = sizeof(INITCOMMONCONTROLSEX);
            initcc.dwICC = ICC_BAR_CLASSES;
            InitCommonControlsEx(&initcc);

            colors[LABELCOLOR] = RGB(200, 30, 50);
            colors[TITLECOLOR] = RGB(0, 0, 0);
            colors[VERTEXCOLOR] = RGB(30, 30, 200);
            colors[LINKCOLOR] = RGB(10, 70, 180);
            colors[BUTTCOLOR] = RGB(216, 191, 216);
            colors[BORDERBUTTCOLOR] = RGB(196, 171, 196);

            labelColorBrush = CreateSolidBrush(colors[LABELCOLOR]);
            titleColorBrush = CreateSolidBrush(colors[TITLECOLOR]);
            vertexColorBrush = CreateSolidBrush(colors[VERTEXCOLOR]);
            linkColorBrush = CreateSolidBrush(colors[LINKCOLOR]);
            btnBrush = CreateSolidBrush(colors[BUTTCOLOR]);
            borderBtnBrush = CreateSolidBrush(colors[BORDERBUTTCOLOR]);

            btnPen = CreatePen(PS_SOLID, 3, colors[BUTTCOLOR]);
            borderBtnPen = CreatePen(PS_SOLID, 3, colors[BORDERBUTTCOLOR]);
            labelColorPen = CreatePen(PS_SOLID, 2, colors[LABELCOLOR]);
            vertexColorPen= CreatePen(PS_SOLID, 2, colors[VERTEXCOLOR]);

            btnVertex = CreateWindow(_T("BUTTON"), _T(""), WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                     10, 10, 50, 50, hwnd, NULL, hinst, NULL);
            btnLabel = CreateWindow(_T("BUTTON"), _T(""), WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                   70, 10, 50, 50, hwnd, NULL, hinst, NULL);
            btnColor = CreateWindow(_T("BUTTON"), _T(""), WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                    130, 10, 50, 50, hwnd, NULL, hinst, NULL);

            btnLabelTip = CreateToolTip(hwnd, hinst);
            btnVertexTip = CreateToolTip(hwnd, hinst);

            labelColorTip = CreateToolTip(hwnd, hinst);
            titleColorTip = CreateToolTip(hwnd, hinst);
            vertexColorTip = CreateToolTip(hwnd, hinst);
            linkColorTip = CreateToolTip(hwnd, hinst);

            AddToolForTip(btnLabelTip, btnLabel, hinst, 0, 0, 0, 0, (LPTSTR)_T("Создать надпись"), TTF_IDISHWND);
            AddToolForTip(btnVertexTip, btnVertex, hinst, 0, 0, 0, 0, (LPTSTR)_T("Создать вершину"), TTF_IDISHWND);

            AddToolForTip(labelColorTip, btnColor, hinst, 0, 20, 25, 35, (LPTSTR)_T("Цвет надписей"), TTF_SUBCLASS | TTF_CENTERTIP);
            AddToolForTip(titleColorTip, btnColor, hinst, 25, 20, 50, 35, (LPTSTR)_T("Цвет заголовков"), TTF_SUBCLASS | TTF_CENTERTIP);
            AddToolForTip(vertexColorTip, btnColor, hinst, 0, 35, 25, 50, (LPTSTR)_T("Цвет вершин или полей"), TTF_SUBCLASS | TTF_CENTERTIP);
            AddToolForTip(linkColorTip, btnColor, hinst, 25, 35, 50, 50, (LPTSTR)_T("Цвет связей или границ"), TTF_SUBCLASS | TTF_CENTERTIP);

            chsclr.Flags = CC_FULLOPEN;
            chsclr.hwndOwner = hwnd;
            chsclr.lpCustColors = (LPDWORD)colors;
            chsclr.lStructSize = sizeof(CHOOSECOLOR);

            mContBtnLabel.track.cbSize = mContBtnVertex.track.cbSize = sizeof(TRACKMOUSEEVENT);
            mContBtnLabel.track.dwFlags = mContBtnVertex.track.dwFlags = TME_LEAVE;
            mContBtnLabel.track.dwHoverTime = mContBtnVertex.track.dwHoverTime = HOVER_DEFAULT;
            mContBtnLabel.track.hwndTrack = btnLabel;
            mContBtnVertex.track.hwndTrack = btnVertex;

            mContBtnLabel.isOver = mContBtnVertex.isOver = FALSE;
            mContBtnLabel.windowTip = btnLabelTip;
            mContBtnVertex.windowTip = btnVertexTip;

            SetWindowSubclass(btnLabel, HoverableWindowProc, 0, (DWORD_PTR)&mContBtnLabel);
            SetWindowSubclass(btnVertex, HoverableWindowProc, 0, (DWORD_PTR)&mContBtnVertex);

            drawer = (Drawer*)lpar;
            return 0;

        case WM_SIZE:
            if((UINT)wpar == SIZE_MINIMIZED) {
                SendMessage(GetParent(hwnd), WM_CHECK_MENU_ITEM, (WPARAM)hwnd, (LPARAM)MF_UNCHECKED);
            } else if((UINT)wpar == SIZE_RESTORED) {
                SendMessage(GetParent(hwnd), WM_CHECK_MENU_ITEM, (WPARAM)hwnd, (LPARAM)MF_CHECKED);
            }
            return 0;

        case WM_COMMAND:
            if(HIWORD(wpar) == BN_CLICKED)
                if(lpar == (LPARAM)btnColor)
                {
                    if(ChooseColor(&chsclr) == TRUE) {
                        UpdateColors();
                        SendMessage(GetParent(hwnd), WM_UPDATE_COLOR, (WPARAM)0, (LPARAM)0);
                    }
                }
                else if(lpar == (LPARAM)btnVertex)
                {
                    DialogBox(hinst, MAKEINTRESOURCE(VERTEXDLG), hwnd, VertexDlgProc);

                    if(isAssignee)
                        MessageBox(NULL, _T("Назначаемые"), _T("Группа"), MB_OK);
                    else
                        MessageBox(NULL, _T("Места"), _T("Группа"), MB_OK);

                    MessageBox(NULL, label, _T("Надпись"), MB_OK);
                }
            return 0;

        case WM_LBUTTONDBLCLK:
            ShowWindow(hwnd, SW_MINIMIZE);
            SendMessage(GetParent(hwnd), WM_REDRAW_AUX_WINDOW, (WPARAM)hwnd, 0);
            return 0;

        case WM_DRAWITEM:
            drawStruct = (LPDRAWITEMSTRUCT)lpar;
            if(drawStruct->hwndItem == btnVertex || drawStruct->hwndItem == btnLabel) {

                if(drawStruct->itemState & ODS_SELECTED) {
                    SelectBrush(drawStruct->hDC, borderBtnBrush);
                } else {
                    SelectBrush(drawStruct->hDC, btnBrush);
                }
                if(((mContBtnLabel.isOver && drawStruct->hwndItem == btnLabel) ||
                        (mContBtnVertex.isOver && drawStruct->hwndItem == btnVertex)) && !(drawStruct->itemState & ODS_SELECTED)) {
                    SelectPen(drawStruct->hDC, btnPen);
                } else {
                    SelectPen(drawStruct->hDC, borderBtnPen);
                }
                Rectangle(drawStruct->hDC, 0, 0, 50, 50);

                if(drawStruct->hwndItem == btnVertex) {
                    SelectPen(drawStruct->hDC, vertexColorPen);
                    Ellipse(drawStruct->hDC, 5, 5, 45, 45);
                } else {
                    SelectPen(drawStruct->hDC, labelColorPen);
                    MoveToEx(drawStruct->hDC, 5, 45, NULL);
                    LineTo(drawStruct->hDC, 25, 5);
                    LineTo(drawStruct->hDC, 45, 45);
                    MoveToEx(drawStruct->hDC, 12, 25, NULL);
                    LineTo(drawStruct->hDC, 38, 25);
                }
            } else if(drawStruct->hwndItem == btnColor) {
                DrawText(drawStruct->hDC, _T("Цвет:"), 6, &drawStruct->rcItem, DT_CENTER | DT_TOP);
                SelectBrush(drawStruct->hDC, labelColorBrush);
                Rectangle(drawStruct->hDC, 0, 20, 25, 35);
                SelectBrush(drawStruct->hDC, titleColorBrush);
                Rectangle(drawStruct->hDC, 25, 20, 50, 35);
                SelectBrush(drawStruct->hDC, vertexColorBrush);
                Rectangle(drawStruct->hDC, 0, 35, 25, 50);
                SelectBrush(drawStruct->hDC, linkColorBrush);
                Rectangle(drawStruct->hDC, 25, 35, 50, 50);
            }
            return 0;

        case WM_DESTROY:
            RemoveWindowSubclass(btnLabel, HoverableWindowProc, 0);
            RemoveWindowSubclass(btnVertex, HoverableWindowProc, 0);

            DeleteBrush(btnBrush);
            DeleteBrush(borderBtnBrush);
            DeleteBrush(labelColorBrush);
            DeleteBrush(titleColorBrush);
            DeleteBrush(vertexColorBrush);
            DeleteBrush(linkColorBrush);

            DeletePen(btnPen);
            DeletePen(borderBtnPen);
            DeletePen(labelColorPen);
            DeletePen(vertexColorPen);
            return 0;

        default:
            return DefWindowProc(hwnd, msg, wpar, lpar);
    }
}

void UpdateColors()
{
    DeleteBrush(labelColorBrush);
    DeleteBrush(titleColorBrush);
    DeleteBrush(vertexColorBrush);
    DeleteBrush(linkColorBrush);
    labelColorBrush = CreateSolidBrush(colors[LABELCOLOR]);
    titleColorBrush = CreateSolidBrush(colors[TITLECOLOR]);
    vertexColorBrush = CreateSolidBrush(colors[VERTEXCOLOR]);
    linkColorBrush = CreateSolidBrush(colors[LINKCOLOR]);
    InvalidateRect(btnColor, NULL, FALSE);

    DeletePen(labelColorPen);
    DeletePen(vertexColorPen);
    labelColorPen = CreatePen(PS_SOLID, 2, colors[LABELCOLOR]);
    vertexColorPen= CreatePen(PS_SOLID, 2, colors[VERTEXCOLOR]);
    InvalidateRect(btnLabel, NULL, FALSE);
    InvalidateRect(btnVertex, NULL, FALSE);
}
