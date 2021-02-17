#ifndef Resources_h
#define Resources_h

#include <windows.h>

#define VERINFO 1
#define TOOLDLG 2
#define IDC_LB1TOOLDLG 1001
#define IDC_LB2TOOLDLG 1002
#define IDC_LB3TOOLDLG 1003
#define IDC_RB1TOOLDLG 1004
#define IDC_RB2TOOLDLG 1005
#define IDC_RB3TOOLDLG 1006
#define IDC_RB4TOOLDLG 1007
#define IDC_RB5TOOLDLG 1008
#define IDC_RB6TOOLDLG 1009
#define IDC_RB7TOOLDLG 1010
#define IDC_OKTOOLDLG 1011
#define IDC_CANCELTOOLDLG 1012
#define VERTEXDLG 3
#define IDC_EDITVERTEXDLG 1013
#define IDC_RB1VERTEXDLG 1014
#define IDC_RB2VERTEXDLG 1015
#define IDC_OKVERTEXDLG 1016

#define ACCELSET 4
//Константы отдельных от меню аксселераторов
#define ZOOMOUT_AC 100
#define ZOOMOUTE_AC 101
#define ZOOMIN_AC 102
#define ZOOMINE_AC 103
#define SHIFTLEFT_AC 104
#define SHIFTTOP_AC 105
#define SHIFTRIGHT_AC 106
#define SHIFTBOTTOM_AC 107

#define MENUSET 5
//Константы меню файл
#define CREATE_MI 200
#define EDIT_MI 201
#define DELETE_MI 202
#define LOAD_MI 203
#define SAVE_MI 204
#define SAVEAS_MI 205
#define CLOSE_MI 206
#define EXIT_MI 207
//Константы меню вид
#define SW_MANAGE_MI 210
#define SW_DRAW_MI 211
//Константы меню помощь
#define ABOUT_MI 230
#define HOTKEY_MI 231
//Константы меню настройки
#define TOOL_MI 240

//Цвета
enum DrawColors
{
    LABELCOLOR,
    TITLECOLOR,
    VERTEXCOLOR,
    LINKCOLOR,
    BUTTCOLOR,
    BORDERBUTTCOLOR
};
extern COLORREF colors[16];

//Шрифты
#define MISTRAL_FONT 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,\
                                    CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SCRIPT, _T("Mistral")
#define SEGOE_SCRIPT_FONT 9, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, RUSSIAN_CHARSET, OUT_DEFAULT_PRECIS,\
                                  CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, FF_SCRIPT, _T("Segoe Script")

#endif // Resources_h
