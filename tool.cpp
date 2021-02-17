#define _WIN32_WINNT 0x0501

#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "resources.h"
#include "tool.h"
#include "hoverableWindow.h"

enum DETAILS paramdetail = STEP;
BOOL paramview = TRUE;
enum ALGS paramalg = VENG_ALG, oldalg;

BOOL algChanged = false;

static HWND btnOk, btnCancel;
static HRGN rndRectRgnL, rndRectRgnR, rndRectRgnDraw;
static HBRUSH btnBrush, borderBtnBrush;
static LPDRAWITEMSTRUCT drawStruct;

static mouseContext mContBtnOk, mContBtnCancel;

BOOL CALLBACK ToolDlgProc(HWND hwnd, UINT msg, WPARAM wpar, LPARAM lpar)
{
    switch(msg)
    {
        case WM_INITDIALOG:
            btnOk = GetDlgItem(hwnd, IDC_OKTOOLDLG);
            btnCancel = GetDlgItem(hwnd, IDC_CANCELTOOLDLG);

            btnBrush = CreateSolidBrush(colors[BUTTCOLOR]);
            borderBtnBrush = CreateSolidBrush(colors[BORDERBUTTCOLOR]);

            if(paramdetail == STEP)
                CheckRadioButton(hwnd, IDC_RB1TOOLDLG, IDC_RB2TOOLDLG, IDC_RB1TOOLDLG);
            else
                CheckRadioButton(hwnd, IDC_RB1TOOLDLG, IDC_RB2TOOLDLG, IDC_RB2TOOLDLG);

            if(paramview)
                CheckRadioButton(hwnd, IDC_RB3TOOLDLG, IDC_RB4TOOLDLG, IDC_RB3TOOLDLG);
            else
                CheckRadioButton(hwnd, IDC_RB3TOOLDLG, IDC_RB4TOOLDLG, IDC_RB4TOOLDLG);

            if(paramalg == VENG_ALG)
                CheckRadioButton(hwnd, IDC_RB5TOOLDLG, IDC_RB7TOOLDLG, IDC_RB5TOOLDLG);
            else if(paramalg == MAXIMIN_ALG)
                CheckRadioButton(hwnd, IDC_RB5TOOLDLG, IDC_RB7TOOLDLG, IDC_RB6TOOLDLG);
            else if(paramalg == MINRISK_ALG)
                CheckRadioButton(hwnd, IDC_RB5TOOLDLG, IDC_RB7TOOLDLG, IDC_RB7TOOLDLG);


            rndRectRgnL = CreateRoundRectRgn(0, 0, 90, 40, 25, 30);
            rndRectRgnR = CreateRoundRectRgn(0, 0, 90, 40, 25, 30);
            rndRectRgnDraw = CreateRoundRectRgn(0, 0, 90, 40, 25, 30);

            SetWindowRgn(btnOk, rndRectRgnL, TRUE);
            SetWindowRgn(btnCancel, rndRectRgnR, TRUE);

            mContBtnOk.track.cbSize = mContBtnCancel.track.cbSize = sizeof(TRACKMOUSEEVENT);
            mContBtnOk.track.dwFlags = mContBtnCancel.track.dwFlags = TME_LEAVE;
            mContBtnOk.track.dwHoverTime = mContBtnCancel.track.dwHoverTime = HOVER_DEFAULT;
            mContBtnOk.track.hwndTrack = btnOk;
            mContBtnCancel.track.hwndTrack = btnCancel;

            mContBtnOk.isOver = mContBtnCancel.isOver = FALSE;

            SetWindowSubclass(btnOk, HoverableWindowProc, 0, (DWORD_PTR)&mContBtnOk);
            SetWindowSubclass(btnCancel, HoverableWindowProc, 0, (DWORD_PTR)&mContBtnCancel);
            return TRUE;

        case WM_COMMAND:
            if(HIWORD(wpar) == BN_CLICKED)
                switch(LOWORD(wpar))
                {
                    case IDC_RB1TOOLDLG:
                    case IDC_RB2TOOLDLG:
                        CheckRadioButton(hwnd, IDC_RB1TOOLDLG, IDC_RB2TOOLDLG, LOWORD(wpar));
                        break;
                    case IDC_RB3TOOLDLG:
                    case IDC_RB4TOOLDLG:
                        CheckRadioButton(hwnd, IDC_RB3TOOLDLG, IDC_RB4TOOLDLG, LOWORD(wpar));
                        break;
                    case IDC_RB5TOOLDLG:
                    case IDC_RB6TOOLDLG:
                    case IDC_RB7TOOLDLG:
                        CheckRadioButton(hwnd, IDC_RB5TOOLDLG, IDC_RB7TOOLDLG, LOWORD(wpar));
                        break;
                    case IDOK:
                    case IDC_OKTOOLDLG:
                        if(IsDlgButtonChecked(hwnd, IDC_RB1TOOLDLG))
                            paramdetail = STEP;
                        else
                            paramdetail = ONCE;

                        paramview = IsDlgButtonChecked(hwnd, IDC_RB3TOOLDLG);

                        oldalg = paramalg;
                        if(IsDlgButtonChecked(hwnd, IDC_RB5TOOLDLG))
                            paramalg = VENG_ALG;
                        else if(IsDlgButtonChecked(hwnd, IDC_RB6TOOLDLG))
                            paramalg = MAXIMIN_ALG;
                        else
                            paramalg = MINRISK_ALG;

                        algChanged = oldalg != paramalg;
                        EndDialog(hwnd, 0);
                        break;
                    case IDCANCEL:
                    case IDC_CANCELTOOLDLG:
                        algChanged = false;
                        EndDialog(hwnd, 0);
                        break;
                }
            return FALSE;

        case WM_DRAWITEM:
            drawStruct = (LPDRAWITEMSTRUCT) lpar;
            if(drawStruct->CtlID == IDC_OKTOOLDLG || drawStruct->CtlID == IDC_CANCELTOOLDLG)
            {
                SetBkMode(drawStruct->hDC, TRANSPARENT);

                if(drawStruct->itemState & ODS_SELECTED)
                    FillRgn(drawStruct->hDC, rndRectRgnDraw, borderBtnBrush);
                else
                    FillRgn(drawStruct->hDC, rndRectRgnDraw, btnBrush);

                if(((drawStruct->CtlID == IDC_OKTOOLDLG && mContBtnOk.isOver) ||
                    (drawStruct->CtlID == IDC_CANCELTOOLDLG && mContBtnCancel.isOver)) && !(drawStruct->itemState & ODS_SELECTED))
                    FrameRgn(drawStruct->hDC, rndRectRgnDraw, btnBrush, 3, 3);
                else
                    FrameRgn(drawStruct->hDC, rndRectRgnDraw, borderBtnBrush, 3, 3);

                if(drawStruct->CtlID == IDC_OKTOOLDLG)
                    DrawText(drawStruct->hDC, _T("Ок"), 3, &drawStruct->rcItem, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
                else
                    DrawText(drawStruct->hDC, _T("Отмена"), 7, &drawStruct->rcItem, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
            }
            return FALSE;

        case WM_DESTROY:
            DeleteRgn(rndRectRgnL);
            DeleteRgn(rndRectRgnR);
            DeleteRgn(rndRectRgnDraw);

            DeleteBrush(btnBrush);
            DeleteBrush(borderBtnBrush);

            RemoveWindowSubclass(btnOk, HoverableWindowProc, 0);
            RemoveWindowSubclass(btnCancel, HoverableWindowProc, 0);
            return FALSE;

        default:
            return FALSE;
    }
}
