#ifndef StepInfo_h
#define StepInfo_h

#include <tchar.h>

enum ActionType
{
    NOTACTION,
    SELECTVERT,
    SELECTLINK,
    SETVERTMARKER,
    SETLINKMARKER,
    SETWEIGHT
};

struct ActionInfo {
    ActionType action;
    unsigned lparam;
    unsigned mparam;
    unsigned rparam;
    ActionInfo(ActionType action, unsigned lparam, unsigned mparam, unsigned rparam)
        : action(action), lparam(lparam), mparam(mparam), rparam(rparam)
    {};
};

struct StepInfo {
    ActionInfo actionInfo;
    TCHAR nameStep[64];
    TCHAR descryptStep[2048];
    StepInfo(ActionType action, unsigned lparam, unsigned mparam, unsigned rparam)
        : actionInfo(action, lparam, mparam, rparam)
    {};
};

#endif // StepInfo_h
