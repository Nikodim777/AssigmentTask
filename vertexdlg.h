#ifndef VERTEXDLG_H_INCLUDED
#define VERTEXDLG_H_INCLUDED

#warning TODO (�#1#): add owner to messagebox
#warning TODO (�#1#): add macros for easyier fill mousecontext structure
#warning TODO (�#1#): update dialogs
#warning TODO (�#3#): Convert BOOL to bool
#warning TODO (�#3#): Replace VertexGroup to bool isAssignee
extern BOOL isAssignee;
extern TCHAR label[8];

BOOL CALLBACK VertexDlgProc(HWND hwnd, UINT msg, WPARAM wpar, LPARAM lpar);

#endif // VERTEXDLG_H_INCLUDED
