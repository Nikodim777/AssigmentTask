#include "MaxiMinAlg.h"

void MaxiMinAlg::Solve(Example *curExample)
{
    int szMatrixX = curExample->numPlaces, szMatrixY = curExample->numAssignees,
        numIteration = min(curExample->numAssignees, curExample->numPlaces);
    int i, j, cnt, assigns[numIteration][2], *tmpMax, sum = 0;
    int minInRow[szMatrixY], minInCol[szMatrixX], indexMinInRow[szMatrixY], indexMinInCol[szMatrixX];
    bool choosedRow[szMatrixY] = {false}, choosedCol[szMatrixX] = {false};
    TCHAR buffer[256];

    for(cnt = 0; cnt < numIteration; cnt++)
    {
        for(i = 0; i < szMatrixY; i++)
            if(!choosedRow[i])
            {
                minInRow[i] = INT_MAX;
                for(j = 0; j < szMatrixX; j++)
                    if(!choosedCol[j] && (minInRow[i] == INT_MAX || curExample->matrix[i * szMatrixX + j] < minInRow[i]))
                    {
                        minInRow[i] = curExample->matrix[i * szMatrixX + j];
                        indexMinInRow[i] = j;
                    }
            }
            else
                minInRow[i] = -1;
        if(paramdetail == STEP)
        {
            stepsInfo.push_back(new StepInfo(NOTACTION, 0, 0, 0));
            _tcscpy(stepsInfo.back()->nameStep, _T("Выбор мин. эл. строк"));
            _tcscpy(stepsInfo.back()->descryptStep, _T("Минимальные элементы строк:\n"));
            for(i = 0; i < szMatrixY; i++)
            {
                _itot(minInRow[i], buffer, 10);
                _tcscat(stepsInfo.back()->descryptStep, buffer);
                if(i == szMatrixY - 1)
                    _tcscat(stepsInfo.back()->descryptStep, _T(";"));
                else
                    _tcscat(stepsInfo.back()->descryptStep, _T(", "));
            }
        }

        for(i = 0; i < szMatrixX; i++)
            if(!choosedCol[i])
            {
                minInCol[i] = INT_MAX;
                for(j = 0; j < szMatrixY; j++)
                    if(!choosedRow[j] && (minInCol[i] == INT_MAX || curExample->matrix[j * szMatrixX + i] < minInCol[i]))
                    {
                        minInCol[i] = curExample->matrix[j * szMatrixX + i];
                        indexMinInCol[i] = j;
                    }
            }
            else
                minInCol[i] = -1;
        if(paramdetail == STEP)
        {
            stepsInfo.push_back(new StepInfo(NOTACTION, 0, 0, 0));
            _tcscpy(stepsInfo.back()->nameStep, _T("Выбор мин. эл. столбцов"));
            _tcscpy(stepsInfo.back()->descryptStep, _T("Минимальные элементы столбцов:\n"));
            for(i = 0; i < szMatrixX; i++)
            {
                _itot(minInCol[i], buffer, 10);
                _tcscat(stepsInfo.back()->descryptStep, buffer);
                if(i == szMatrixX - 1)
                    _tcscat(stepsInfo.back()->descryptStep, _T(";"));
                else
                    _tcscat(stepsInfo.back()->descryptStep, _T(", "));
            }
        }

        assigns[cnt][0] = max_element(minInRow, minInRow + szMatrixY) - minInRow;
        assigns[cnt][1] = indexMinInRow[assigns[cnt][0]];

        tmpMax = max_element(minInCol, minInCol + szMatrixX);
        if(*tmpMax > curExample->matrix[assigns[cnt][0] * szMatrixX + assigns[cnt][1]])
        {
            assigns[cnt][1] = tmpMax - minInCol;
            assigns[cnt][0] = indexMinInCol[assigns[cnt][1]];
        }

        if(paramdetail == STEP)
        {
            stepsInfo.push_back(new StepInfo(NOTACTION, 0, 0, 0));
            _tcscpy(stepsInfo.back()->nameStep, _T("Назначение"));
            _stprintf(stepsInfo.back()->descryptStep, _T("%s назначается на %s\n"),
                        curExample->vertexLabels.at(assigns[cnt][0]), curExample->vertexLabels.at(szMatrixY + assigns[cnt][1]));
        }

        choosedRow[assigns[cnt][0]] = true;
        choosedCol[assigns[cnt][1]] = true;
        sum += curExample->matrix[assigns[cnt][0] * szMatrixX + assigns[cnt][1]];
    }


    stepsInfo.push_back(new StepInfo(NOTACTION, 0, 0, 0));
    _tcscpy(stepsInfo.back()->nameStep, _T("Решение"));
    _tcscpy(stepsInfo.back()->descryptStep, _T(""));
    for(i = 0; i < numIteration; i++)
    {
        _stprintf(buffer, _T("%s назначается на %s\n"),
                                                            curExample->vertexLabels.at(assigns[i][0]), curExample->vertexLabels.at(szMatrixY + assigns[i][1]));
        _tcscat(stepsInfo.back()->descryptStep, buffer);
    }
    _tcscat(stepsInfo.back()->descryptStep, _T("Цена: "));
    _itot(sum, buffer, 10);
    _tcscat(stepsInfo.back()->descryptStep, buffer);
}

void MaxiMinAlg::ClearSolution()
{
    for(StepInfo* info : stepsInfo)
        delete info;
    stepsInfo.clear();
}

StepInfo* MaxiMinAlg::GetStepInfo(int *numStep)
{
    if(*numStep >= 0 && *numStep < (int)stepsInfo.size())
        return stepsInfo.at(*numStep);
    else
    {
        if(*numStep < 0)
            *numStep = 0;
        else
            *numStep = (int)stepsInfo.size() - 1;
        return NULL;
    }
}
