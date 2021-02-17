#ifndef DRAWTOOLWNDdrawToolWnd_H_INCLUDED
#define DRAWTOOLWNDdrawToolWnd_H_INCLUDED

//Размеры
#define WIDTH_DRAWTOOL_WND 188
#define HEIGHT_DRAWTOOL_WND 100
//Пользовательские сообщения
#define WM_REDRAW_AUX_WINDOW WM_USER
#define WM_CHECK_MENU_ITEM WM_USER + 1
#define WM_UPDATE_COLOR WM_USER + 2

LRESULT CALLBACK DrawToolWndProc(HWND, UINT, WPARAM, LPARAM);

void UpdateColors();

#endif // DRAWTOOLWND_H_INCLUDED
