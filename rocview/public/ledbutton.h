/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef LEDBUTTON_H_
#define LEDBUTTON_H_

#include <wx/graphics.h>
#include <wx/panel.h>

class LEDButton : public wxPanel
{
    wxWindow* Parent;
    bool pressedDown;
    bool useLED;
    bool textOnly;
    wxString text;

    int buttonWidth;
    int buttonHeight;

    wxBitmap* icon;
    wxBitmap* scaledIcon;


public:
    bool ON;
    LEDButton(wxWindow* parent, wxString text, int width, int height, bool useLED=true, bool textOnly=false);
    ~LEDButton();

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);

    void setLED(bool ON);
    void SetLabel(const wxString &text);
    void SetIcon(wxBitmap* icon);
    void SetValue(const wxString& value);
    bool SetBackgroundColour(const wxColour& colour);
    void Display7Segement(wxGraphicsContext* gc);
    void draw7Segement(wxGraphicsContext* gc, int val, int offset);

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





#endif /* LEDBUTTON_H_ */
