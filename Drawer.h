#ifndef Drawer_h
#define Drawer_h

#include <algorithm>
#include <math.h>
#include <tchar.h>
#include <vector>
#include <windows.h>
#include <windowsx.h>
#include "resources.h"
#include "drawToolWnd.h"
#include "Example.h"
#include "StepInfo.h"

//Пользовательские сообщения
#define WM_UPDATE_SCALE_STATUS_PART WM_USER + 3

using namespace std;

enum DrawerState
{
    ASMATRIX,
    ASGRAPH,
    EDITASMATRIX,
    EDITASGRAPH
};

class Drawer {
    class DrawableComponent
    {
        protected:
            Drawer *const drawer;

        public:
            DrawableComponent(Drawer *const drawer) : drawer(drawer) {};
            virtual bool IsPointIn(POINT point) const = 0;
            virtual void OnMouseClick();
            virtual void NormalizeView();
            virtual void Paint(HDC paintDC, unsigned scale, LPRECT paintRect) const;
            virtual ~DrawableComponent(){};
    };

    class Vertex : public DrawableComponent
    {
        enum VertexState
        {
            STANDART,
            SELECTED,
            EDITABLE
        };
        VertexState state;
        unsigned number;
        POINT posInGraph;
        POINT posInMatrix;
        TCHAR label[8];
        TCHAR mark[2];

        public:
            static const unsigned width = 80;

            Vertex(Drawer* drawer, unsigned number, POINT posInGraph, POINT posInMatrix, TCHAR label[8]);
            unsigned GetNumber() const;
            void DecrementNumber();
            POINT GetPosInGraph() const ;
            POINT GetPosInMatrix() const ;
            bool IsPointIn(POINT point) const;
            void OnMouseClick();
            void NormalizeView();
            void Paint(HDC paintDC, unsigned scale, LPRECT paintRect) const ;
    };

    class Link : public DrawableComponent
    {
        enum LinkState
        {
            STANDART,
            CHANGED,
            SELECTED,
            EDITABLE
        };
        LinkState state;
        Vertex* startVertex;
        Vertex* endVertex;
        TCHAR mark[2];

        public:
            static const unsigned width = 3;
            int weight;

            Link(Drawer* drawer, Vertex* startVertex, Vertex* endVertex, int  weight);
            POINT GetNumVertices() const;
            bool IsPointIn(POINT point) const;
            void OnMouseClick();
            void NormalizeView();
            void Paint(HDC paintDC, unsigned scale, LPRECT paintRect) const ;
    };

    class Label : public DrawableComponent
    {
        POINT pos;
        RECT rect;

        TCHAR label[256];

        public:
            Label(Drawer* drawer, POINT pos, TCHAR label[256]);

            LPTSTR GetLabel() const;
            bool IsPointIn(POINT point) const;
            void OnMouseClick();
            void NormalizeView();
            void Paint(HDC paintDC, unsigned scale, LPRECT paintRect) const;
    };

    enum VertexGroup
    {
        ASSIGNEE = 0,
        PLACE = 1
    };

    DrawerState state;
    unsigned numAssignees, numPlaces;
    vector<Vertex*> vertices;
    vector<Link*> links;
    vector<Label*> labels;
    Vertex* selectedVertex;
    Link* selectedLink;
    Label* selectedLabel;
    unsigned vertexHDistance, vertexVDistance;
    unsigned scale;
    POINT origin;
    RECT mainRect;
    Example* context;

    static const unsigned titleFontSize = 44, labelFontSize = 33;

    HDC paintDC, memDC;
    HBITMAP memBitmap;
    HWND hwnd;
    HFONT titleFont, labelFont;
    HPEN linkPen, selectedLinkPen;

    bool ChangeScale(int value);
    void ScaleFont();
    void ScaleRect(LPRECT rect);

    public:
        Drawer(HWND hwnd);

        void SetContext(Example* context);
        void ClearContext();

        void SetState(DrawerState state);

        void AddVertex(VertexGroup group, TCHAR label[8]);
        void AddLink(Vertex* startVertex, Vertex* endVertex, int  weight);
        void AddLabel(POINT pos, TCHAR label[256]);
        void DeleteVertex();
        void DeleteLink();
        void DeleteLabel();

        void MakeInteractive(ActionInfo& action);

        void WriteChangesContext() const ;

        void OnMouseClick(POINT clickPoint);
        bool OnAccel(WORD accel);
        void RedrawBuffer(LPRECT wndRect);
        void Paint();
        void UpdatePens();

        ~Drawer();
};

#endif // Drawer_h
