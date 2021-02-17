#ifndef MaxiMinAlg_h
#define MaxiMinAlg_h

#include <stdlib.h>
#include "AssignTaskAlgorithm.h"


class MaxiMinAlg : public AssignTaskAlgorithm {
    public:
        void Solve(Example *curExample);
        void ClearSolution();
        StepInfo* GetStepInfo(int *numStep);
};

#endif // MaxiMinAlg_h
