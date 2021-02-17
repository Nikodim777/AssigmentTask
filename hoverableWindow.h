#ifndef HOVERABLEWINDOW_H_INCLUDED
#define HOVERABLEWINDOW_H_INCLUDED

typedef struct
{
    BOOL isOver;
    HWND windowTip;
    TRACKMOUSEEVENT track;
} mouseContext;

LRESULT CALLBACK HoverableWindowProc(HWND hwnd, UINT msg, WPARAM wpar, LPARAM lpar, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

#endif // HOVERABLEWINDOW_H_INCLUDED
