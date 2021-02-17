#include "Example.h"

Example::Example()
    : numAssignees(0), numPlaces(0), matrix(NULL), posLabels(NULL), colorVertex(colors[VERTEXCOLOR]), colorLink(colors[LINKCOLOR]),
            colorLabel(colors[LABELCOLOR]), colorTitle(colors[TITLECOLOR]), changed(false)
{
    name[0] = L'\0';
    descrypt[0] = L'\0';
    titleAssignees[0] = L'\0';
    titlePlaces[0] = L'\0';
}

Example::Example(LPCTSTR filename)
    : changed(false)
{
    TCHAR shortBuffer[256], buffer[2048];
   unsigned i,j;

    FILE* file = _tfopen(filename, _T("r, ccs=UTF-8"));

    //Получение основных сведений о примере
    if(_ftscanf(file, _T("%255[^\n]\n%2047[^🍄]🍄\n%255[^\n]\n%255[^\n]\n"), name, descrypt, titleAssignees, titlePlaces) != 4)
        MessageBox(NULL, _T("Ошибка при чтении общих сведений"), _T("Ошибка"), MB_OK | MB_ICONERROR);

    //Получение сведений об исходной матрице
    if(_ftscanf(file, _T("%u %u\n"), &numAssignees, &numPlaces) != 2)
            MessageBox(NULL, _T("Ошибка при чтении размеров матрицы"), _T("Ошибка"), MB_OK | MB_ICONERROR);
    matrix = (int*) malloc(sizeof(int) * numAssignees * numPlaces);
    if(matrix == NULL)
        MessageBox(NULL, _T("Ошибка выделения памяти под матрицу"), _T("Ошибка"), MB_OK | MB_ICONERROR);
    for(i = 0; i < numAssignees; i++)
    {
        for(j = 0; j < numPlaces - 1; j++)
            if(_ftscanf(file, _T("%d "),  &matrix[i * numPlaces + j]) != 1)
                MessageBox(NULL, _T("Ошибка при чтении элемента матрицы"), _T("Ошибка"), MB_OK | MB_ICONERROR);
        if(_ftscanf(file, _T("%d\n"), &matrix[i * numPlaces + j]) != 1)
            MessageBox(NULL, _T("Ошибка при чтении элемента матрицы"), _T("Ошибка"), MB_OK | MB_ICONERROR);
    }

    //Получение сведений о надписях вершин
    for(i = 0; i < (numAssignees + numPlaces); i++)
    {
        if(_ftscanf(file, _T("%7[^\n]\n"), shortBuffer) != 1)
            MessageBox(NULL, _T("Ошибка при чтении надписи вершины"), _T("Ошибка"), MB_OK | MB_ICONERROR);
        vertexLabels.push_back(_tcsdup(shortBuffer));
    }

    //Получение сведений о надписях
    while(1)
    {
        if(_ftscanf(file, _T("%255[^\n]\n"), shortBuffer) != 1)
            MessageBox(NULL, _T("Ошибка при чтении надписи"), _T("Ошибка"), MB_OK | MB_ICONERROR);
        if(!_tcscmp(shortBuffer, _T("🍺")))
            break;
        labels.push_back(_tcsdup(shortBuffer));
    }

    //Получение сведений о координатах надписей
    posLabels = (LPPOINT)malloc(sizeof(POINT) * labels.size());
    if(posLabels == NULL)
        MessageBox(NULL, _T("Ошибка выделения памяти под координаты надписей"), _T("Ошибка"), MB_OK | MB_ICONERROR);
    for(i = 0; i < labels.size(); i++)
        if(_ftscanf(file, _T("%d %d\n"), &posLabels[i].x, &posLabels[i].y) != 2)
            MessageBox(NULL, _T("Ошибка при чтении координат надписи"), _T("Ошибка"), MB_OK | MB_ICONERROR);

    //Получение сведений о цветах
    if(_ftscanf(file, _T("%x %x %x %x\n"), &colors[VERTEXCOLOR], &colors[LINKCOLOR], &colors[LABELCOLOR], &colors[TITLECOLOR]) != 4)
        MessageBox(NULL, _T("Ошибка при чтении информации о цветах"), _T("Ошибка"), MB_OK | MB_ICONERROR);

#ifdef TEST_EXAMPLE_LOAD
    MessageBox(NULL, name, _T("Имя"), MB_OK);
    MessageBox(NULL, descrypt, _T("Описание"), MB_OK);
    MessageBox(NULL, titleAssignees, _T("Заголовок назначаемых"), MB_OK);
    MessageBox(NULL, titlePlaces, _T("Заголовок мест"), MB_OK);

    buffer[0] =  _T('\0');
    for(i = 0; i < numAssignees; i++)
    {
        for(j = 0; j < numPlaces - 1; j++)
        {
            _stprintf(shortBuffer, _T("%d "), matrix[i * numPlaces + j]);
            _tcscat(buffer, shortBuffer);
        }
        _stprintf(shortBuffer, _T("%d\n"), matrix[i * numPlaces + j]);
        _tcscat(buffer, shortBuffer);
    }
    MessageBox(NULL, buffer, _T("Матрица"), MB_OK);

    for(LPTSTR label : vertexLabels)
        MessageBox(NULL, label, _T("Надписи вершин"), MB_OK);

    for(LPTSTR label : labels)
        MessageBox(NULL, label, _T("Надпись"), MB_OK);

    buffer[0] =  _T('\0');
    for(i = 0; i < labels.size(); i++)
    {
        _stprintf(shortBuffer, _T("%d %d\n"), posLabels[i].x, posLabels[i].y);
        _tcscat(buffer, shortBuffer);
    }
    MessageBox(NULL, buffer, _T("Координаты надписей"), MB_OK);

    _stprintf(buffer, _T("%X %X %X %X\n"), colors[VERTEXCOLOR], colors[LINKCOLOR], colors[LABELCOLOR], colors[TITLECOLOR]);
    MessageBox(NULL, buffer, _T("Цвета"), MB_OK);

#endif //TEST_EXAMPLE_LOAD

    fclose(file);
}

void Example::Save(LPCTSTR filename) const
{
    unsigned i,j;
    FILE* file = _tfopen(filename, _T("w, ccs=UTF-8"));

    _ftprintf(file, _T("%s\n%s🍄\n%s\n%s\n"), name, descrypt, titleAssignees, titlePlaces);

    _ftprintf(file, _T("%u %u\n"), numAssignees, numPlaces);
    for(i = 0; i < numAssignees; i++)
    {
        for(j = 0; j < numPlaces - 1; j++)
            _ftprintf(file, _T("%d "),  matrix[i * numPlaces + j]);
        _ftprintf(file, _T("%d\n"), matrix[i * numPlaces + j]);
    }

    for(LPTSTR label : vertexLabels)
        _ftprintf(file, _T("%s\n"), label);

    for(LPTSTR label : labels)
        _ftprintf(file, _T("%s\n"), label);
    _ftprintf(file, _T("🍺\n"));

    for(i = 0; i < labels.size(); i++)
        _ftprintf(file, _T("%d %d\n"), posLabels[i].x, posLabels[i].y);

    _ftprintf(file, _T("%X %X %X %X\n"), colors[VERTEXCOLOR], colors[LINKCOLOR], colors[LABELCOLOR], colors[TITLECOLOR]);

    fclose(file);
}

Example::~Example()
{
    free(matrix);
    for(LPTSTR label : vertexLabels)
        free((void*)label);
    for(LPTSTR label : labels)
        free((void*)label);
    free(posLabels);
}
