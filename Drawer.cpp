#define _WIN32_WINNT 0x0501

#include "Drawer.h"

Drawer::Vertex::Vertex(Drawer* drawer, unsigned number, POINT posInGraph, POINT posInMatrix, TCHAR label[8])
    : DrawableComponent(drawer), state(VertexState::STANDART), number(number), posInGraph(posInGraph), posInMatrix(posInMatrix)
{
    _tcscpy(this->label, label);
    _tcscpy(this->mark, _T(" "));

    free((void*)label);
}

unsigned Drawer::Vertex::GetNumber() const
{
    return number;
}

void Drawer::Vertex::DecrementNumber()
{
    if(number != 0)
        number--;
}

POINT Drawer::Vertex::GetPosInGraph() const
{
    return posInGraph;
}

POINT Drawer::Vertex::GetPosInMatrix() const
{
    return posInMatrix;
}

bool Drawer::Vertex::IsPointIn(POINT point) const
{
    if(drawer->state == DrawerState::ASGRAPH)
        return hypot((int)(point.x - (posInGraph.x + width / 2) * (LONG)drawer->scale / 100),
                            (int)(point.y - (posInGraph.y + width / 2) * (LONG)drawer->scale / 100))  <= width / 2 * (LONG)drawer->scale / 100;
    else
        return point.x >= posInMatrix.x * (LONG)drawer->scale / 100 && point.x < (posInMatrix.x + (LONG)width + 10) * (LONG)drawer->scale / 100 &&
                    point.y >= posInMatrix.y * (LONG)drawer->scale / 100 && point.y < (posInMatrix.y + (LONG)width / 2) * (LONG)drawer->scale / 100;
}

void Drawer::Vertex::OnMouseClick()
{
    state = VertexState::SELECTED;
    drawer->selectedVertex = this;
}

void Drawer::Vertex::NormalizeView()
{
    state = VertexState::STANDART;
}

void Drawer::Vertex::Paint(HDC paintDC, unsigned scale, LPRECT paintRect) const
{
    RECT tmpRect;
    TCHAR buffer[16];

    if(state == VertexState::SELECTED)
        SetDCBrushColor(paintDC, colors[LINKCOLOR]);

    if(drawer->state == DrawerState::ASGRAPH)
    {
        SetRect(&tmpRect, paintRect->left + posInGraph.x * (int)scale / 100, paintRect->top + posInGraph.y * (int)scale / 100,
                    paintRect->left + (posInGraph.x + width) * (int)scale / 100, paintRect->top + (posInGraph.y + width) * (int)scale / 100);
        Ellipse(paintDC, tmpRect.left, tmpRect.top, tmpRect.right, tmpRect.bottom);

        DrawText(paintDC, label, _tcslen(label), &tmpRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
        tmpRect.left += (tmpRect.right - tmpRect.left) / 2;
        tmpRect.bottom -= (tmpRect.bottom - tmpRect.top) / 2;
        DrawText(paintDC, mark, _tcslen(mark), &tmpRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
    }
    else
    {
        SetRect(&tmpRect, paintRect->left + posInMatrix.x * (int)scale / 100, paintRect->top + posInMatrix.y * (int)scale / 100,
            paintRect->left + (posInMatrix.x + (width + 10)) * (int)scale / 100, paintRect->top + (posInMatrix.y + width / 2) * (int)scale / 100);
        Rectangle(paintDC, tmpRect.left, tmpRect.top, tmpRect.right, tmpRect.bottom);

        _stprintf(buffer, _T("%s%s"), label, mark);
        DrawText(paintDC, buffer, _tcslen(buffer), &tmpRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
    }

    if(state == VertexState::SELECTED)
        SetDCBrushColor(paintDC, colors[VERTEXCOLOR]);
}


Drawer::Link::Link(Drawer* drawer, Vertex* startVertex, Vertex* endVertex, int  weight)
    : DrawableComponent(drawer), state(LinkState::STANDART), startVertex(startVertex), endVertex(endVertex), weight(weight)
{
    _tcscpy(this->mark, _T(" "));
}

POINT Drawer::Link::GetNumVertices() const
{
    return (POINT){(LONG)startVertex->GetNumber(), (LONG)endVertex->GetNumber()};
}

bool Drawer::Link::IsPointIn(POINT point) const
{
    POINT posStartVertex, posEndVertex, linkStart, linkEnd;

    if(drawer->state == DrawerState::ASGRAPH)
    {
        posStartVertex = startVertex->GetPosInGraph(), posEndVertex = endVertex->GetPosInGraph();
        linkStart.x = (posStartVertex.x + Vertex::width) * (int)drawer->scale / 100;
        linkStart.y = (posStartVertex.y + Vertex::width / 2) * (int)drawer->scale / 100;
        linkEnd.x = posEndVertex.x * (int)drawer->scale / 100;
        linkEnd.y = (posEndVertex.y + Vertex::width / 2) * (int)drawer->scale / 100;

        int A = linkStart.y - linkEnd.y;
        int B = linkEnd.x - linkStart.x;
        int C = linkStart.x * linkEnd.y - linkStart.y * linkEnd.x;

        return abs((A * point.x + B * point.y + C)) / hypot(A, B) <= width * drawer->scale / 100 && point.x >= linkStart.x && point.x <= linkEnd.x;
    }
    else
    {
        posStartVertex = startVertex->GetPosInMatrix(), posEndVertex = endVertex->GetPosInMatrix();
        return point.x >= posEndVertex.x * (LONG)drawer->scale / 100 &&
                    point.x < (posEndVertex.x + (LONG)Vertex::width + 10) * (LONG)drawer->scale / 100 &&
                    point.y >= posStartVertex.y * (LONG)drawer->scale / 100 &&
                    point.y < (posStartVertex.y + (LONG)Vertex::width / 2) * (LONG)drawer->scale / 100;
    }

}

void Drawer::Link::OnMouseClick()
{
    state = LinkState::SELECTED;
    drawer->selectedLink = this;
}

void Drawer::Link::NormalizeView()
{
    state = LinkState::STANDART;
}

void Drawer::Link::Paint(HDC paintDC, unsigned scale, LPRECT paintRect) const
{
    RECT tmpRect;
    POINT posStartVertex, posEndVertex;
    TCHAR buffer[16];

    if(drawer->state == DrawerState::ASGRAPH)
    {
        HPEN oldPen;
        POINT linkStart, linkEnd;
        posStartVertex = startVertex->GetPosInGraph();
        posEndVertex = endVertex->GetPosInGraph();

        if(state == LinkState::SELECTED)
            oldPen = SelectPen(paintDC, drawer->selectedLinkPen);

        linkStart.x = paintRect->left + (posStartVertex.x + Vertex::width) * (int)scale / 100;
        linkStart.y = paintRect->top + (posStartVertex.y + Vertex::width / 2) * (int)scale / 100;
        linkEnd.x = paintRect->left + posEndVertex.x * (int)scale / 100;
        linkEnd.y = paintRect->top + (posEndVertex.y + Vertex::width / 2) * (int)scale / 100;

        MoveToEx(paintDC, linkStart.x, linkStart.y, NULL);
        LineTo(paintDC, linkEnd.x, linkEnd.y);

        tmpRect.left = tmpRect.right = linkStart.x + (linkEnd.x - linkStart.x) / 6;
        tmpRect.top = tmpRect.bottom = linkStart.y + (linkEnd.y - linkStart.y) / 6 - 15;

        _stprintf(buffer, _T("%d%1s"), weight, mark);
        DrawText(paintDC, buffer, _tcslen(buffer), &tmpRect,  DT_SINGLELINE | DT_NOCLIP);

        if(state == LinkState::SELECTED)
            SelectObject(paintDC, oldPen);
    }
    else
    {
        posStartVertex = startVertex->GetPosInMatrix();
        posEndVertex = endVertex->GetPosInMatrix();

        if(state == LinkState::SELECTED)
            SetDCBrushColor(paintDC, colors[LINKCOLOR]);

        SetRect(&tmpRect, paintRect->left + posEndVertex.x * (int)scale / 100, paintRect->top +  posStartVertex.y * (int)scale / 100,
            paintRect->left + (posEndVertex.x + Vertex::width + 10) * (int)scale / 100,
            paintRect->top + (posStartVertex.y + Vertex::width / 2) * (int)scale / 100);
        Rectangle(paintDC, tmpRect.left, tmpRect.top, tmpRect.right, tmpRect.bottom);

        _stprintf(buffer, _T("%d%1s"), weight, mark);
        DrawText(paintDC, buffer, _tcslen(buffer), &tmpRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);

        if(state == LinkState::SELECTED)
            SetDCBrushColor(paintDC, colors[VERTEXCOLOR]);
    }
}


Drawer::Label::Label(Drawer* drawer, POINT pos, TCHAR label[256])
    : DrawableComponent(drawer),  pos(pos)
{
    _tcscpy(this->label, label);

    free((void*)label);
}

LPTSTR Drawer::Label::GetLabel() const
{
    return (LPTSTR)label;
}

bool Drawer::Label::IsPointIn(POINT point) const
{
    RECT rect;

    SetRect(&rect, pos.x * (int)drawer->scale / 100, pos.y * (int)drawer->scale / 100, 0, 0);
    DrawText(drawer->memDC, label, _tcslen(label), &rect, DT_SINGLELINE | DT_CALCRECT);

    if(drawer->state == DrawerState::ASGRAPH)
        return PtInRect(&rect, point);
    else
        return false;
}

void Drawer::Label::OnMouseClick()
{
    MessageBox(NULL, label, _T("Надпись"), MB_OK);
}

void Drawer::Label::NormalizeView()
{}

void Drawer::Label::Paint(HDC paintDC, unsigned scale, LPRECT paintRect) const
{
    TextOut(paintDC, paintRect->left + pos.x * (int)scale / 100, paintRect->top + pos.y * (int)scale / 100, label, _tcslen(label));
}



Drawer::Drawer(HWND hwnd)
    : state(DrawerState::ASGRAPH), numAssignees(0), numPlaces(0), vertexHDistance(0), vertexVDistance(0),
        scale(100), origin{0, 0}, hwnd(hwnd)
{
    titleFont = CreateFont(titleFontSize, MISTRAL_FONT);
    labelFont = CreateFont(labelFontSize, MISTRAL_FONT);
}

void Drawer::SetContext(Example* context)
{
    unsigned i,j;

    this->context = context;

    numAssignees = context->numAssignees;
    numPlaces = context->numPlaces;

    vertexHDistance = min(400u * max(context->numAssignees + context->numPlaces, 20u) / 20u, 800u);
    vertexVDistance = min(150u * max(context->numAssignees + context->numPlaces, 20u) / 20u, 300u);

    for(i = 0; i < context->numAssignees; i++)
        vertices.push_back(new Vertex(this, i, (POINT){50l, 100l + (LONG)i * (LONG)vertexVDistance},
                                                    (POINT){50l, 100l + (LONG)(i + 1) * (LONG)Vertex::width / 2}, _tcsdup(context->vertexLabels.at(i))));
    for(i = 0; i < context->numPlaces; i++)
        vertices.push_back(new Vertex(this, i, (POINT){50l + (LONG)vertexHDistance, 100l + (LONG)i * (LONG)vertexVDistance},
                                                    (POINT){50l + (LONG)(i + 1) * (LONG)(Vertex::width + 10), 100l},
                                                    _tcsdup(context->vertexLabels.at(numAssignees + i))));

    for(i = 0; i < context->numAssignees; i++)
        for(j = 0; j < context->numPlaces; j++)
            links.push_back(new Link(this, vertices.at(i), vertices.at(context->numAssignees + j), context->matrix[i * context->numPlaces + j]));

    for(i = 0; i < context->labels.size(); i++)
        labels.push_back(new Label(this, context->posLabels[i], _tcsdup(context->labels.at(i))));

    UpdateColors();
    SendMessage(hwnd, WM_UPDATE_SCALE_STATUS_PART, (WPARAM)scale, (LPARAM)0);

    linkPen = CreatePen(PS_SOLID, Link::width * scale / 100, colors[LINKCOLOR]);
    selectedLinkPen = CreatePen(PS_SOLID, Link::width * scale / 100, colors[VERTEXCOLOR]);
}

void Drawer::ClearContext()
{
    context = NULL;

    for(Vertex* vertex : vertices)
        delete vertex;
    for(Link* link : links)
        delete link;
    for(Label* label : labels)
        delete label;

    vertices.clear();
    links.clear();
    labels.clear();

    DeletePen(linkPen);
    DeletePen(selectedLinkPen);
}

void Drawer::SetState(DrawerState state)
{
    this->state = state;
}

bool Drawer::ChangeScale(int value)
{
    if((scale <= 30 && value <= 0) || (scale >= 300 && value >= 0))
        return false;

    if(value < 0 && value < 30 - (int)scale)
        value = 30 - scale;
    else if(value > 0 && value > 300 -(int) scale)
        value = 300 - scale;

    scale += value;

    if(scale >= 70)
        ScaleFont();

    SendMessage(hwnd, WM_UPDATE_SCALE_STATUS_PART, (WPARAM)scale, (LPARAM)0);
    return true;
}

void Drawer::ScaleFont()
{
    DeleteFont(titleFont);
    DeleteFont(labelFont);
    titleFont = CreateFont(titleFontSize * scale / 100, MISTRAL_FONT);
    labelFont = CreateFont(labelFontSize * scale / 100, MISTRAL_FONT);
}

void Drawer::ScaleRect(LPRECT rect)
{
    rect->left = rect->left * (int)scale / 100;
    rect->top = rect->top * (int)scale / 100;
    rect->right = rect->right * (int)scale / 100;
    rect->bottom = rect->bottom * (int)scale / 100;
}

void Drawer::AddVertex(VertexGroup group, TCHAR label[8])
{
    unsigned num = (group == ASSIGNEE ? numAssignees : numPlaces);
    context->changed = true;
    vertices.emplace(vertices.begin() + (group == ASSIGNEE ? numAssignees : numAssignees + numPlaces),
                        new Vertex(this, num,
                                        (POINT){50l + (group == ASSIGNEE ? 0 : (LONG)vertexHDistance), 100l + (LONG)num * (LONG)vertexVDistance},
                                        (POINT){50l + (group == ASSIGNEE ? 0 : (LONG)(num + 1) * (LONG)(Vertex::width + 10)),
                                                    100l + (group == ASSIGNEE ? (LONG)(num + 1) * (LONG)Vertex::width / 2 : 0)}, label));
    group == ASSIGNEE ? numAssignees++ : numPlaces++;
}

void Drawer::AddLink(Vertex* startVertex, Vertex* endVertex, int weight)
{
    context->changed = true;
    links.push_back(new Link(this, startVertex, endVertex, weight));
}

void Drawer::AddLabel(POINT pos, TCHAR label[8])
{
    context->changed = true;
    labels.push_back(new Label(this, pos, label));
}

void Drawer::DeleteVertex()
{
    unsigned index;
    context->changed = true;
    if(selectedVertex != NULL)
    {
        auto selectedVertexIterator = find(vertices.begin(), vertices.end(), selectedVertex);
        index = selectedVertexIterator - vertices.begin();

        delete selectedVertex;
        vertices.erase(selectedVertexIterator);
        selectedVertex = NULL;

        if(index + 1 < numAssignees)
            for(unsigned i = index + 1; i < numAssignees; i++)
                vertices.at(i)->DecrementNumber();
        else if(index >= numAssignees)
            for(unsigned i = index + 1; i < numPlaces; i++)
                vertices.at(i)->DecrementNumber();
    }
}

void Drawer::DeleteLink()
{
    context->changed = true;
    if(selectedLink != NULL)
    {
        auto selectedLinkIterator = find(links.begin(), links.end(), selectedLink);
        delete selectedLink;
        links.erase(selectedLinkIterator);
        selectedLink = NULL;
    }
}

void Drawer::DeleteLabel()
{
    context->changed = true;
    if(selectedLabel != NULL)
    {
        auto selectedLabelIterator = find(labels.begin(), labels.end(), selectedLabel);
        delete selectedLabel;
        labels.erase(selectedLabelIterator);
        selectedLabel = NULL;
    }
}

void Drawer::MakeInteractive(ActionInfo& action)
{
}

void Drawer::WriteChangesContext() const
{
    POINT numVertices;

    context->numAssignees = numAssignees;
    context->numPlaces = numPlaces;

    free(context->matrix);
    context->matrix = (int*) malloc(sizeof(int) * numAssignees * numPlaces);
    ZeroMemory(context->matrix, sizeof(int) * numAssignees * numPlaces);

    for(Link* link : links)
    {
        numVertices = link->GetNumVertices();
        context->matrix[numVertices.x * numPlaces + numVertices.y] = link->weight;
    }

    for(LPTSTR label : context->labels)
        free((void*)label);
    for(Label* label : labels)
        context->labels.push_back(_tcsdup(label->GetLabel()));

    context->changed = false;
}

void Drawer::OnMouseClick(POINT clickPoint)
{
    bool isFind = false;

    for(Vertex* vertex : vertices)
        if(!isFind && vertex->IsPointIn((POINT){clickPoint.x - mainRect.left, clickPoint.y -  mainRect.top}))
        {
            isFind = true;
            vertex->OnMouseClick();
        }
        else
            vertex->NormalizeView();

    for(Link* link : links)
        if(!isFind && link->IsPointIn((POINT){clickPoint.x - mainRect.left, clickPoint.y -  mainRect.top}))
        {
            isFind = true;
            link->OnMouseClick();
        }
        else
            link->NormalizeView();

    if(state == DrawerState::ASGRAPH)
        for(Label* label : labels)
            if(!isFind && label->IsPointIn((POINT){clickPoint.x - mainRect.left, clickPoint.y -  mainRect.top}))
            {
                isFind = true;
                label->OnMouseClick();
            }
            else
                label->NormalizeView();
}

bool Drawer::OnAccel(WORD accel)
{
    bool ret = true;

    if(accel == ZOOMOUT_AC)
        ret = ChangeScale(-10);
    else if(accel == ZOOMOUTE_AC)
        ret = ChangeScale(-50);
    else if(accel == ZOOMIN_AC)
        ret = ChangeScale(10);
    else if(accel == ZOOMINE_AC)
        ret = ChangeScale(50);
    else if(accel == SHIFTLEFT_AC)
        origin.x += 10;
    else if(accel == SHIFTTOP_AC)
        origin.y += 10;
    else if(accel == SHIFTRIGHT_AC)
        origin.x -= 10;
    else if(accel == SHIFTBOTTOM_AC)
        origin.y -= 10;

    if(ret)
        UpdatePens();

    return ret;
}

void Drawer::RedrawBuffer(LPRECT wndRect)
{
    LONG oldTop;
    RECT nameRect, descryptRect;
    TCHAR buffer[1024];

    DeleteDC(memDC);
    DeleteBitmap(memBitmap);

    paintDC = GetDC(hwnd);
    memDC = CreateCompatibleDC(paintDC);
    memBitmap = CreateCompatibleBitmap(paintDC, wndRect->right - wndRect->left, wndRect->bottom - wndRect->top);
    SelectObject(memDC, memBitmap);

    FillRect(memDC, wndRect, (HBRUSH)COLOR_BACKGROUND);
    if(context != NULL)
    {
        SetBkMode(memDC, TRANSPARENT);

        SetRect(&nameRect, 0, 0, wndRect->right, 0);
        oldTop = nameRect.top;
        OffsetRect(&nameRect,  origin.x, origin.y);
        ScaleRect(&nameRect);

        SelectFont(memDC, titleFont);
        SetTextColor(memDC, colors[TITLECOLOR]);
        oldTop = DrawText(memDC, context->name, _tcslen(context->name), &nameRect, DT_CENTER | DT_CALCRECT) * 100 / scale + oldTop;
        DrawText(memDC, context->name, _tcslen(context->name), &nameRect, DT_CENTER);

        SetRect(&descryptRect, 0, oldTop, wndRect->right, 0);
        OffsetRect(&descryptRect,  origin.x, origin.y);
        ScaleRect(&descryptRect);

        SelectFont(memDC, labelFont);
        SetTextColor(memDC, colors[LABELCOLOR]);
        oldTop = DrawText(memDC, context->descrypt, _tcslen(context->descrypt), &descryptRect, DT_CENTER | DT_CALCRECT) * 100 / scale + oldTop;
        DrawText(memDC, context->descrypt, _tcslen(context->descrypt), &descryptRect, DT_CENTER);

        SetRect(&mainRect, 0, oldTop, wndRect->right, wndRect->bottom);
        OffsetRect(&mainRect,  origin.x, origin.y);
        ScaleRect(&mainRect);

        if(state == ASGRAPH)
        {
            TextOut(memDC, mainRect.left + 50 * (int)scale / 100,
                                    mainRect.top + (100 - 60) * (int)scale / 100, context->titleAssignees, _tcslen(context->titleAssignees));
            TextOut(memDC, mainRect.left + (50 + vertexHDistance) * (int)scale / 100,
                                    mainRect.top + (100 - 60) * (int)scale / 100, context->titlePlaces, _tcslen(context->titlePlaces));

            SelectBrush(memDC, GetStockBrush(DC_BRUSH));
            SetDCBrushColor(memDC, colors[VERTEXCOLOR]);
            for(Vertex* vertex : vertices)
                vertex->Paint(memDC, scale, &mainRect);

            SelectPen(memDC, linkPen);
            for(Link* link : links)
                link->Paint(memDC, scale, &mainRect);

            for(Label* label : labels)
                label->Paint(memDC, scale, &mainRect);
        }
        else if(state == ASMATRIX)
        {
            _stprintf(buffer, _T("%s \\ %s"), context->titleAssignees, context->titlePlaces);
            TextOut(memDC, mainRect.left + 50 * (int)scale / 100, mainRect.top + (100 - 60) * (int)scale / 100, buffer, _tcslen(buffer));

            SelectBrush(memDC, GetStockBrush(DC_BRUSH));
            SetDCBrushColor(memDC, colors[VERTEXCOLOR]);
            SelectPen(memDC, linkPen);

            for(Vertex* vertex : vertices)
                vertex->Paint(memDC, scale, &mainRect);

            for(Link* link : links)
                link->Paint(memDC, scale, &mainRect);
        }
    }

    ReleaseDC(hwnd, paintDC);
}

void Drawer::Paint()
{
    PAINTSTRUCT paintStruct;
    paintDC = BeginPaint(hwnd, &paintStruct);
    BitBlt(paintDC, paintStruct.rcPaint.left, paintStruct.rcPaint.top,
                    paintStruct.rcPaint.right - paintStruct.rcPaint.left, paintStruct.rcPaint.bottom - paintStruct.rcPaint.top,
                    memDC, paintStruct.rcPaint.left, paintStruct.rcPaint.top, SRCCOPY);
    EndPaint(hwnd, &paintStruct);
}

void Drawer::UpdatePens()
{
    DeletePen(linkPen);
    linkPen = CreatePen(PS_SOLID, Link::width * scale / 100, colors[LINKCOLOR]);
    DeletePen(selectedLinkPen);
    selectedLinkPen = CreatePen(PS_SOLID, Link::width * scale / 100, colors[VERTEXCOLOR]);
}

Drawer::~Drawer()
{
    for(Vertex* vertex : vertices)
        delete vertex;
    for(Link* link : links)
        delete link;
    for(Label* label : labels)
        delete label;

    DeleteFont(titleFont);
    DeleteFont(labelFont);
    DeletePen(linkPen);
    DeletePen(selectedLinkPen);
}
