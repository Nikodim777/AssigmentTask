#ifndef Example_h
#define Example_h

#define __STDC_FORMAT_MACROS

#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <windows.h>
#include "resources.h"

using namespace std;

struct Example {
    TCHAR name[256], descrypt[2048], titleAssignees[256], titlePlaces[256];
    unsigned numAssignees, numPlaces;
    int * matrix;
    vector<LPTSTR>  vertexLabels, labels;
    LPPOINT posLabels;
    COLORREF colorVertex, colorLink, colorLabel, colorTitle;
    bool changed;

    Example();
    Example(LPCTSTR filename);
    void Save(LPCTSTR filename) const;
    ~Example();
};

#endif // Example_h
