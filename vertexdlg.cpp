#define _WIN32_WINNT 0x0501

#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "resources.h"
#include "vertexdlg.h"
#include "hoverableWindow.h"

BOOL isAssignee = true;
TCHAR label[8] = _T("");

static HWND btnOk;
static HRGN rndRectRgn, rndRectRgnDraw;
static HBRUSH btnBrush, borderBtnBrush;
static LPDRAWITEMSTRUCT drawStruct;

static mouseContext mContBtnOk;

BOOL CALLBACK VertexDlgProc(HWND hwnd, UINT msg, WPARAM wpar, LPARAM lpar)
{
    switch(msg)
    {
        case WM_INITDIALOG:
            btnOk = GetDlgItem(hwnd, IDC_OKVERTEXDLG);

            btnBrush = CreateSolidBrush(colors[BUTTCOLOR]);
            borderBtnBrush = CreateSolidBrush(colors[BORDERBUTTCOLOR]);

            if(isAssignee)
                CheckRadioButton(hwnd, IDC_RB1VERTEXDLG, IDC_RB2VERTEXDLG, IDC_RB1VERTEXDLG);
            else
                CheckRadioButton(hwnd, IDC_RB1VERTEXDLG, IDC_RB2VERTEXDLG, IDC_RB2VERTEXDLG);

            rndRectRgn = CreateRoundRectRgn(0, 0, 80, 40, 25, 28);
            rndRectRgnDraw = CreateRoundRectRgn(0, 0, 80, 40, 25, 28);

            SetWindowRgn(btnOk, rndRectRgn, TRUE);

            mContBtnOk.track.cbSize = sizeof(TRACKMOUSEEVENT);
            mContBtnOk.track.dwFlags = TME_LEAVE;
            mContBtnOk.track.dwHoverTime = HOVER_DEFAULT;
            mContBtnOk.track.hwndTrack = btnOk;

            mContBtnOk.isOver = FALSE;

            SetWindowSubclass(btnOk, HoverableWindowProc, 0, (DWORD_PTR)&mContBtnOk);
            return TRUE;

        case WM_COMMAND:
            if(HIWORD(wpar) == BN_CLICKED)
                switch(LOWORD(wpar))
                {
                    case IDC_RB1VERTEXDLG:
                    case IDC_RB2VERTEXDLG:
                        CheckRadioButton(hwnd, IDC_RB1VERTEXDLG, IDC_RB2VERTEXDLG, LOWORD(wpar));
                        break;

                    case IDOK:
                    case IDC_OKVERTEXDLG:
                        GetDlgItemText(hwnd, IDC_EDITVERTEXDLG, label, 8);
                        isAssignee = IsDlgButtonChecked(hwnd, IDC_RB1VERTEXDLG);

                        EndDialog(hwnd, 0);
                        break;

                    case IDCANCEL:
                        EndDialog(hwnd, 0);
                        break;
                }
            return FALSE;

        case WM_DRAWITEM:
            drawStruct = (LPDRAWITEMSTRUCT) lpar;

            SetBkMode(drawStruct->hDC, TRANSPARENT);

            if(drawStruct->itemState & ODS_SELECTED)
                FillRgn(drawStruct->hDC, rndRectRgnDraw, borderBtnBrush);
            else
                FillRgn(drawStruct->hDC, rndRectRgnDraw, btnBrush);

            if(drawStruct->CtlID == IDC_OKVERTEXDLG && mContBtnOk.isOver && !(drawStruct->itemState & ODS_SELECTED))
                FrameRgn(drawStruct->hDC, rndRectRgnDraw, btnBrush, 3, 3);
            else
                FrameRgn(drawStruct->hDC, rndRectRgnDraw, borderBtnBrush, 3, 3);

            DrawText(drawStruct->hDC, _T("Ок"), 3, &drawStruct->rcItem, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
            return FALSE;

        case WM_DESTROY:
            DeleteRgn(rndRectRgn);
            DeleteRgn(rndRectRgnDraw);

            DeleteBrush(btnBrush);
            DeleteBrush(borderBtnBrush);

            RemoveWindowSubclass(btnOk, HoverableWindowProc, 0);
            return FALSE;

        default:
            return FALSE;
    }
}
