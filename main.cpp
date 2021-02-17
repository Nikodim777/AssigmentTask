#define _WIN32_WINNT 0x0501

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>
#include <commctrl.h>
#include "drawToolWnd.h"
#include "Drawer.h"
#include "Example.h"
#include "manageToolWnd.h"
#include "resources.h"
#include "tool.h"

#define WND_WIDTH 800
#define WND_HEIGHT 600
#define BUFSIZE 256
#define LARGEBUFSIZE 1024
#define IDICON 282
#define HEIGHT_STATUS_BAR 30

//Идентификатор строки статуса
#define STATUS_BAR 200
#define NUM_PARTS_STATUS_BAR 3
#define FILENAMESTATUSPART 0
#define REGIMSTATUSPART 1
#define SCALESTATUSPART 2

LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

static void RegAuxWindow(WNDPROC auxWndProc, LPTSTR auxClName);
static void ToggleAuxWindow(HWND auxWindow, int MENUITEM, int iconicLeft, int width, int height);
static void ShowNormalAuxWindow(HWND auxWindow, int width, int height);
static void ShowIconicAuxWindow(HWND auxWindow, int iconicLeft);
static void EnableDrawMode(BOOL val);
static void EnableManageMode(BOOL val);
static void EnableEditMenuItems(BOOL val);
static void EnableSaveMenuItems(BOOL val);
static bool IsCloseUnsaveExample();

static TCHAR clName[] = _T("AssigmentTask");
static TCHAR manageToolWndClName[] = _T("manageAuxWnd");
static TCHAR drawToolWndClName[] = _T("drawAuxWnd");
static TCHAR title[] = _T("УСАПР для решения задач назначения");
static TCHAR labelManageRegim[] = _T("Режим решения");
static TCHAR labelDrawRegim[] = _T("Режим рисования");
static TCHAR delFileDlgTitle[] = _T("Какой файл желаете удалить?");
static TCHAR openFileDlgTitle[] = _T("Какой файл желаете открыть?");
static TCHAR saveFileDlgTitle[] = _T("Куда желаете сохранить?");
static TCHAR errbuf[BUFSIZE];
static TCHAR scalebuf[16];

HINSTANCE hinst;
static HMENU mainmenu, file, view, help;
static HWND hwnd, status, manageToolWnd, drawToolWnd;
static HACCEL haccel;

static BOOL isAdmin;
static SID_IDENTIFIER_AUTHORITY sid = SECURITY_NT_AUTHORITY;
static PSID adminGroup;

static RECT mainWndRect;
static int statusBarBorders[NUM_PARTS_STATUS_BAR];

static OPENFILENAME ofn = {0};
static TCHAR filepath[LARGEBUFSIZE] = _T(""), filename[BUFSIZE] = _T("");

static Example *curExample;
static Drawer *drawer;

int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpArgs, int nCmd)
{
    MSG msg;
    WNDCLASSEX wc = {0};
    BOOL bRet;

    HINSTANCE hshell = LoadLibrary(_T("SHELL32.dll"));
    if(hshell == NULL)
    {
        _stprintf(errbuf, _T("Ошибка загрузки библиотеки иконок - %d"), (int)GetLastError());
        MessageBox(NULL, errbuf, _T("Ошибка!"), MB_OK | MB_ICONERROR);
    }

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
    wc.hIcon = LoadIcon (hshell, MAKEINTRESOURCE(IDICON));
    if(wc.hIcon == NULL)
    {
        _stprintf(errbuf, _T("Ошибка загрузки иконки - %d"), (int)GetLastError());
        MessageBox(NULL, errbuf, _T("Ошибка!"), MB_OK | MB_ICONERROR);
    }
    wc.hInstance = hInst;
    wc.lpfnWndProc = WinProc;
    wc.lpszClassName = clName;

    if (!RegisterClassEx(&wc))
    {
        _stprintf(errbuf, _T("Ошибка при регистрации класса окна - %d"), (int)GetLastError());
        MessageBox(NULL, errbuf, _T("Ошибка!"), MB_OK | MB_ICONERROR);
        return 1;
    }

    mainmenu = LoadMenu(hInst, MAKEINTRESOURCE(MENUSET));
    file = GetSubMenu(mainmenu, 0);
    view = GetSubMenu(mainmenu, 1);
    help = GetSubMenu(mainmenu, 2);

    hwnd = CreateWindowEx(0, clName, title, WS_OVERLAPPEDWINDOW | WS_TABSTOP | WS_VISIBLE | WS_CLIPCHILDREN,
           CW_USEDEFAULT, CW_USEDEFAULT, WND_WIDTH, WND_HEIGHT,
           NULL, mainmenu, hInst, NULL);
    if(hwnd == NULL)
    {
        _stprintf(errbuf, _T("Ошибка при создании окна - %d"), (int)GetLastError());
        MessageBox(NULL, errbuf, _T("Ошибка!"), MB_OK | MB_ICONERROR);
        return 1;
    }

    haccel = LoadAccelerators(hInst, MAKEINTRESOURCE(ACCELSET));
    if(haccel == NULL)
    {
        _stprintf(errbuf, _T("Ошибка при загрузке горячих клавиш - %d"), (int)GetLastError());
        MessageBox(NULL, errbuf, _T("Ошибка!"), MB_OK | MB_ICONERROR);
        return 1;
    }

    hinst = hInst;

    while((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if(bRet == -1)
        {
            _stprintf(errbuf, _T("Ошибка при получении сообщения - %d"), (int)GetLastError());
            MessageBox(NULL, errbuf, _T("Ошибка!"), MB_OK | MB_ICONERROR);
            return 1;
        }
        else if(!TranslateAccelerator(hwnd, haccel, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return msg.wParam;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wpar, LPARAM lpar)
{
    switch(msg)
    {
        case WM_CREATE:
            //Определение является ли пользователь администратором
            isAdmin = AllocateAndInitializeSid(&sid, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
                                               0, 0, 0, 0, 0, 0, &adminGroup);
            if(isAdmin)
            {
                if(!CheckTokenMembership(NULL, adminGroup, &isAdmin))
                    isAdmin = FALSE;
                FreeSid(adminGroup);
            }

            //Затеменение недоступных пунктов меню
            if(!isAdmin)
            {
                EnableMenuItem(file, CREATE_MI, MF_GRAYED);
                EnableMenuItem(file, EDIT_MI, MF_GRAYED);
                EnableMenuItem(file, DELETE_MI, MF_GRAYED);
                EnableMenuItem(file, SAVE_MI, MF_GRAYED);
            }

            drawer = new Drawer(hwnd);

            status = CreateStatusWindow(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, _T(""), hwnd, STATUS_BAR);

            RegAuxWindow(DrawToolWndProc, (LPTSTR)drawToolWndClName);
            drawToolWnd = CreateWindow(drawToolWndClName, _T("Рисование"), WS_CHILD | WS_VISIBLE |  WS_CAPTION |
                                    WS_MINIMIZE | WS_CLIPSIBLINGS, 0, 0, WIDTH_DRAWTOOL_WND, HEIGHT_DRAWTOOL_WND,
                                    hwnd, NULL, hinst, drawer);
            EnableWindow(drawToolWnd, FALSE);
            EnableMenuItem(view, SW_DRAW_MI, MF_GRAYED);

            RegAuxWindow(ManageToolWndProc, (LPTSTR)manageToolWndClName);
            manageToolWnd = CreateWindow(manageToolWndClName, _T("Управление"), WS_CHILD | WS_VISIBLE | WS_CAPTION |
                                    WS_MINIMIZE | WS_CLIPSIBLINGS, 0, 0, WIDTH_MANAGETOOL_WND, HEIGHT_MANAGETOOL_WND,
                                    hwnd, NULL, hinst, NULL);
            EnableWindow(manageToolWnd, FALSE);
            EnableMenuItem(view, SW_MANAGE_MI, MF_GRAYED);


            ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NONETWORKBUTTON | OFN_DONTADDTORECENT | OFN_NOCHANGEDIR;
            ofn.hwndOwner = hwnd;
            ofn.lpstrDefExt = _T("ase");
            ofn.lpstrFile = filepath;
            ofn.lpstrFileTitle = filename;
            ofn.lpstrFilter = _T("Файлы примера(*.ase)\0*.ase\0\0");
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.nMaxFile = LARGEBUFSIZE;
            ofn.nMaxFileTitle = BUFSIZE;
            return 0;

        case WM_SIZE:
            GetClientRect(hwnd, &mainWndRect);

            MoveWindow(status, 0, HIWORD(lpar) - HEIGHT_STATUS_BAR, LOWORD(lpar), HEIGHT_STATUS_BAR, TRUE);
            if(IsIconic(manageToolWnd))
                MoveWindow(manageToolWnd, 0, HIWORD(lpar) - 50, 100, HEIGHT_STATUS_BAR, TRUE);
            if(IsIconic(drawToolWnd))
                MoveWindow(drawToolWnd, 165, HIWORD(lpar) - 50, 100, HEIGHT_STATUS_BAR, TRUE);

            statusBarBorders[0] = (mainWndRect.right - mainWndRect.left) / 2;
            statusBarBorders[1] =  3 * (mainWndRect.right - mainWndRect.left) / 4;
            statusBarBorders[2] = -1;
            SendMessage(status, SB_SETPARTS, (WPARAM)NUM_PARTS_STATUS_BAR, (LPARAM)statusBarBorders);

            drawer->RedrawBuffer(&mainWndRect);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_PAINT:
            drawer->Paint();
            return 0;

        case WM_LBUTTONDOWN:
            drawer->OnMouseClick((POINT){(LONG)GET_X_LPARAM(lpar), (LONG)GET_Y_LPARAM(lpar)});
            drawer->RedrawBuffer(&mainWndRect);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_COMMAND:
            if(HIWORD(wpar) == 0 || HIWORD(wpar) == 1)
            {
                switch(LOWORD(wpar))
                {
                    case CREATE_MI:
                        if(curExample != NULL && !IsCloseUnsaveExample())
                            break;

                        curExample = new Example();
                        curExample->changed = true;
                        drawer->SetContext(curExample);
                        drawer->RedrawBuffer(&mainWndRect);
                        InvalidateRect(hwnd, NULL, TRUE);

                        EnableManageMode(FALSE);
                        EnableDrawMode(TRUE);

                        EnableEditMenuItems(TRUE);
                        EnableSaveMenuItems(TRUE);

                        SendMessage(manageToolWnd, WM_CLEAR_SOLUTION, 0, 0);
                        SendMessage(status, SB_SETTEXT, (WPARAM)FILENAMESTATUSPART, (LPARAM)_T("Без названия"));
                        break;

                    case EDIT_MI:
                        EnableManageMode(FALSE);
                        EnableDrawMode(TRUE);

                        EnableSaveMenuItems(TRUE);
                        break;

                    case DELETE_MI:
                        filepath[0] = '\0';
                        ofn.lpstrTitle = delFileDlgTitle;
                        if(GetOpenFileName(&ofn))
                            if(_wremove(filepath) == -1 && errno == EACCES)
                                MessageBox(NULL, _T("Нет прав на удаление"),  _T("Ошибка удаления"), MB_OK | MB_ICONERROR);
                        break;

                    case LOAD_MI:
                        if(curExample != NULL && !IsCloseUnsaveExample())
                            break;

                        filepath[0] = '\0';
                        ofn.lpstrTitle = openFileDlgTitle;
                        if(GetOpenFileName(&ofn))
                        {
                            curExample = new Example(filepath);
                            drawer->SetContext(curExample);

                            EnableDrawMode(FALSE);
                            EnableManageMode(TRUE);

                            EnableEditMenuItems(TRUE);

                            SendMessage(manageToolWnd, WM_CLEAR_SOLUTION, 0, 0);
                            SendMessage(manageToolWnd, WM_PRESOLVE, (WPARAM)curExample, 0);
                            SendMessage(status, SB_SETTEXT, (WPARAM)FILENAMESTATUSPART, (LPARAM)filepath);

                            drawer->RedrawBuffer(&mainWndRect);
                            InvalidateRect(hwnd, NULL, TRUE);
                        }
                        break;

                    case SAVE_MI:
                    case SAVEAS_MI:
                        ofn.lpstrTitle = saveFileDlgTitle;
                        if(filepath[0] == '\0' || LOWORD(wpar) == SAVEAS_MI)
                        {
                            filepath[0] = '\0';
                            GetSaveFileName(&ofn);
                        }
                        if(curExample->changed)
                            drawer->WriteChangesContext();
                        curExample->Save(filepath);

                        SendMessage(status, SB_SETTEXT, (WPARAM)FILENAMESTATUSPART, (LPARAM)filepath);
                        MessageBox(hwnd, _T("Файл успешно сохранён!"), _T("Информация"), MB_OK | MB_ICONINFORMATION);
                        break;

                    case CLOSE_MI:
                        if(curExample->changed && !IsCloseUnsaveExample())
                            break;

                        delete curExample;
                        curExample = NULL;
                        drawer->ClearContext();

                        EnableDrawMode(FALSE);
                        EnableManageMode(FALSE);

                        EnableEditMenuItems(FALSE);
                        EnableSaveMenuItems(FALSE);

                        SendMessage(status, SB_SETTEXT, (WPARAM)FILENAMESTATUSPART, (LPARAM)_T(""));
                        SendMessage(status, SB_SETTEXT, (WPARAM)REGIMSTATUSPART, (LPARAM)_T(""));
                        SendMessage(status, SB_SETTEXT, (WPARAM)SCALESTATUSPART, (LPARAM)_T(""));

                        drawer->RedrawBuffer(&mainWndRect);
                        InvalidateRect(hwnd, NULL, TRUE);

                        SendMessage(manageToolWnd, WM_CLEAR_SOLUTION, 0, 0);
                        break;

                    case EXIT_MI:
                        if(!curExample || !curExample->changed || IsCloseUnsaveExample())
                            SendMessage(hwnd, WM_CLOSE, 0, 0);
                        break;

                    case SW_MANAGE_MI:
                        ToggleAuxWindow(manageToolWnd, SW_MANAGE_MI, 0, WIDTH_MANAGETOOL_WND, HEIGHT_MANAGETOOL_WND);
                        break;

                    case SW_DRAW_MI:
                        ToggleAuxWindow(drawToolWnd, SW_DRAW_MI, 165, WIDTH_DRAWTOOL_WND, HEIGHT_DRAWTOOL_WND);
                        break;

                    case TOOL_MI:
                        DialogBox(hinst, MAKEINTRESOURCE(TOOLDLG), hwnd, ToolDlgProc);

                        if(algChanged && curExample)
                        {
                            SendMessage(manageToolWnd, WM_CLEAR_SOLUTION, 0, 0);
                            SendMessage(manageToolWnd, WM_PRESOLVE, (WPARAM)curExample, 0);
                        }

                        if(paramview)
                        {
                            drawer->SetState(DrawerState::ASGRAPH);
                            drawer->RedrawBuffer(&mainWndRect);
                            InvalidateRect(hwnd, NULL, TRUE);
                        }
                        else
                        {
                            drawer->SetState(DrawerState::ASMATRIX);
                            drawer->RedrawBuffer(&mainWndRect);
                            InvalidateRect(hwnd, NULL, TRUE);
                        }

                        if(paramalg == VENG_ALG)
                            MessageBox(NULL, _T("Венгерский алгоритм"), _T("Алгоритм"), MB_OK | MB_ICONINFORMATION);
                        else if(paramalg == MAXIMIN_ALG)
                            MessageBox(NULL, _T("МаксиМин алгоритм"), _T("Алгоритм"), MB_OK | MB_ICONINFORMATION);
                        else if(paramalg == MINRISK_ALG)
                            MessageBox(NULL, _T("Алгоритм минимального риска"), _T("Алгоритм"), MB_OK | MB_ICONINFORMATION);

                        break;

                    case ABOUT_MI:
                        MessageBox(NULL, _T("Эта программа - учебная САПР\nдля решения задачи о назначениях"),
                                   _T("О программе"), MB_OK | MB_ICONINFORMATION);
                        break;

                    case HOTKEY_MI:
                        MessageBox(NULL, _T("ALT + N - Создать пример\nALT + E - Редактировать пример\n"
                                         "ALT + L - Загрузить пример\nALT + S - Cохранить пример\n"
                                         "ALT + C - Закрыть пример\nALT + Q - Выйти\n"
                                         "ALT + M - Показать окно управления\nALT + D - Показать окно рисования\n"
                                         "ALT + T - Настройки\nALT + H - Горячие клавиши\n"
                                         "COMMA - Увеличить\nSHIFT + COMMA - Намного увеличить\n"
                                         "PERIOD - Уменьшить\nSHIFT + PERIOD - Намного уменьшить\n"
                                         "Arrows - Перемещение\n"),
                                   _T("Горячие клавиши"), MB_OK | MB_ICONINFORMATION);
                        break;

                    default:
                        if(HIWORD(wpar))
                            if(curExample != NULL && drawer->OnAccel(LOWORD(wpar)))
                            {
                                drawer->RedrawBuffer(&mainWndRect);
                                InvalidateRect(hwnd, NULL, TRUE);
                            }
                }
            }
            return 0;

        case WM_REDRAW_AUX_WINDOW:
            if((HWND)wpar == manageToolWnd)
                MoveWindow(manageToolWnd, 0,  mainWndRect.bottom - mainWndRect.top - 50,
                        100, HEIGHT_STATUS_BAR, TRUE);
            else
                MoveWindow(drawToolWnd, 165,  mainWndRect.bottom - mainWndRect.top - 50,
                        100, HEIGHT_STATUS_BAR, TRUE);
            return 0;

        case WM_CHECK_MENU_ITEM:
            if((HWND)wpar == manageToolWnd)
                CheckMenuItem(view, SW_MANAGE_MI, (BOOL)lpar);
            else if((HWND)wpar == drawToolWnd)
                CheckMenuItem(view, SW_DRAW_MI, (BOOL)lpar);
            return 0;

        case WM_UPDATE_COLOR:
            drawer->UpdatePens();
            drawer->RedrawBuffer(&mainWndRect);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_UPDATE_SCALE_STATUS_PART:
            _stprintf(scalebuf, _T("Масштаб - %u%%"), (unsigned)wpar);
            SendMessage(status, SB_SETTEXT, (WPARAM)SCALESTATUSPART, (LPARAM)scalebuf);
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_DESTROY:
            DestroyMenu(file);
            DestroyMenu(view);
            DestroyMenu(help);
            DestroyMenu(mainmenu);

            delete curExample;
            delete drawer;

            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, msg, wpar, lpar);
    }
}


void RegAuxWindow(WNDPROC auxWndProc, LPTSTR auxClName)
{
    WNDCLASSEX flwnd = {0};

    flwnd.hbrBackground = (HBRUSH)COLOR_WINDOW;
    flwnd.hInstance = hinst;
    flwnd.lpfnWndProc = auxWndProc;
    flwnd.lpszClassName = auxClName;
    flwnd.cbSize = sizeof(WNDCLASSEX);
    flwnd.style = CS_DBLCLKS;

    if(!RegisterClassEx(&flwnd))
    {
        _stprintf(errbuf, _T("Ошибка при регистрации класса дочернего окна - %d"), (int)GetLastError());
        MessageBox(NULL, errbuf, _T("Ошибка!"), MB_OK | MB_ICONERROR);
    }
}

void ToggleAuxWindow(HWND auxWindow, int MENUITEM, int iconicLeft, int width, int height)
{
    if(IsIconic(auxWindow))
        ShowNormalAuxWindow(auxWindow, width, height);
    else
        ShowIconicAuxWindow(auxWindow, iconicLeft);
}

void ShowNormalAuxWindow(HWND auxWindow, int width, int height)
{
    ShowWindow(auxWindow, SW_RESTORE);
    MoveWindow(auxWindow, mainWndRect.right - width, 0, width, height, TRUE);
}

void ShowIconicAuxWindow(HWND auxWindow, int iconicLeft)
{
        ShowWindow(auxWindow, SW_MINIMIZE);
        MoveWindow(auxWindow, iconicLeft,  mainWndRect.bottom - mainWndRect.top - 50,
                    100, HEIGHT_STATUS_BAR, TRUE);
}

void EnableDrawMode(BOOL val)
{
    EnableMenuItem(view, SW_DRAW_MI, val ? MF_ENABLED : MF_GRAYED);
    EnableWindow(drawToolWnd, val);
    if(val)
    {
        ShowNormalAuxWindow(drawToolWnd, WIDTH_DRAWTOOL_WND, HEIGHT_DRAWTOOL_WND);
        SendMessage(status, SB_SETTEXT, (WPARAM)REGIMSTATUSPART, (LPARAM)labelDrawRegim);
    }
    else
        ShowIconicAuxWindow(drawToolWnd, 165);
}

void EnableManageMode(BOOL val)
{
    EnableMenuItem(view, SW_MANAGE_MI, val ? MF_ENABLED : MF_GRAYED);
    EnableWindow(manageToolWnd, val);
    if(val)
    {
        ShowNormalAuxWindow(manageToolWnd, WIDTH_MANAGETOOL_WND, HEIGHT_MANAGETOOL_WND);
        SendMessage(status, SB_SETTEXT, (WPARAM)REGIMSTATUSPART, (LPARAM)labelManageRegim);
    }
    else
        ShowIconicAuxWindow(manageToolWnd, 0);
}

void EnableEditMenuItems(BOOL val)
{
    EnableMenuItem(file, EDIT_MI, isAdmin ? (val ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
    EnableMenuItem(file, CLOSE_MI, val ? MF_ENABLED : MF_GRAYED);
}

void EnableSaveMenuItems(BOOL val)
{
    EnableMenuItem(file, SAVE_MI, isAdmin ? (val ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
    EnableMenuItem(file, SAVEAS_MI, isAdmin ? (val ? MF_ENABLED : MF_GRAYED) : MF_GRAYED);
}

bool IsCloseUnsaveExample()
{
    if(MessageBox(NULL, _T("Закрыть предыдущий пример не сохранив?"), _T("Предупреждение"),
                                                                    MB_OKCANCEL | MB_ICONEXCLAMATION) == IDCANCEL)
        return false;
    else
    {
        delete curExample;
        curExample = NULL;
        drawer->ClearContext();
    }
    return true;
}
