#ifndef AssignTaskAlgorithm_h
#define AssignTaskAlgorithm_h

#include <algorithm>
#include <limits.h>
#include <stdlib.h>
#include <vector>
#include <tchar.h>
#include <windows.h>
#include "Example.h"
#include "StepInfo.h"
#include "tool.h"

using namespace std;

class AssignTaskAlgorithm{
     protected:
        vector<StepInfo*> stepsInfo;

     public:
        virtual void Solve(Example *curExample) = 0;
        virtual void ClearSolution() = 0;
        virtual StepInfo* GetStepInfo(int *numStep) = 0;
        virtual ~AssignTaskAlgorithm()
        {
            for(StepInfo* info : stepsInfo)
                delete info;
            stepsInfo.clear();
        }
};

#endif // AssignTaskAlgorithm_h
