#ifndef TOOL_H_INCLUDED
#define TOOL_H_INCLUDED

enum DETAILS
{
    STEP,
    ONCE
};

enum ALGS
{
    VENG_ALG,
    MAXIMIN_ALG,
    MINRISK_ALG
};

extern enum DETAILS paramdetail;
extern BOOL paramview;
extern enum ALGS paramalg;

extern BOOL algChanged;

BOOL CALLBACK ToolDlgProc(HWND hwnd, UINT msg, WPARAM wpar, LPARAM lpar);

#endif // TOOL_H_INCLUDED
