/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef SLIDER_H_
#define SLIDER_H_

#include <wx/panel.h>

class Slider : public wxPanel
{
    wxWindow* Parent;
    wxWindow* PrevFocusWindow;
    int Value;
    int Min;
    int Max;
    int Width;
    int Height;
    int ThumbHeight;
    int Move;
    bool Drag;
    int ThumbRange;
    int ThumbOffset;
    int ThumbPos;
    int PrevThumbPos;
    long PrevWheelTime;
    double Step;
    bool InitSet;


public:
    Slider(wxWindow* parent, int width, int height);
    void SetValue(int value, bool force=false);
    int GetValue();
    void SetRange(int minValue, int maxValue);
    void moveThumb(bool forceRefresh=false);
    void OnPaint(wxPaintEvent& event);

    // some useful events
    void mouseMoved(wxMouseEvent& event);
    void mouseDown(wxMouseEvent& event);
    void mouseWheelMoved(wxMouseEvent& event);
    void mouseReleased(wxMouseEvent& event);
    void rightClick(wxMouseEvent& event);
    void mouseLeftWindow(wxMouseEvent& event);
    void keyPressed(wxKeyEvent& event);
    void keyReleased(wxKeyEvent& event);

    DECLARE_EVENT_TABLE()
};


#endif /* SLIDER_H_ */
