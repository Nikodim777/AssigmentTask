#ifndef VERTEXDLG_H_INCLUDED
#define VERTEXDLG_H_INCLUDED

#warning TODO (ÿ#1#): add owner to messagebox
#warning TODO (ÿ#1#): add macros for easyier fill mousecontext structure
#warning TODO (ÿ#1#): update dialogs
#warning TODO (ÿ#3#): Convert BOOL to bool
#warning TODO (ÿ#3#): Replace VertexGroup to bool isAssignee
extern BOOL isAssignee;
extern TCHAR label[8];

BOOL CALLBACK VertexDlgProc(HWND hwnd, UINT msg, WPARAM wpar, LPARAM lpar);

#endif // VERTEXDLG_H_INCLUDED
