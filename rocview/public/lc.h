/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifndef __ROCRAIL_rocgui_LC_H
#define __ROCRAIL_rocgui_LC_H

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "cv.h"
#endif

/*!
 * Includes
 */

#include "wx/tglbtn.h"

#include "rocs/public/node.h"
#include "rocview/public/clock.h"
#include "rocview/public/meter.h"
#include "rocview/public/ledbutton.h"
#include "rocview/public/slider.h"

#define USENEWLOOK

class LC
{
public:
    /// Constructors
    LC( wxPanel* parent );
    void OnButton(wxCommandEvent& event);
    void OnSlider(wxScrollEvent& event);
    void setLocProps( iONode props );
    bool updateLoc( iONode node );
    bool setFX( iONode loco, iONode node );
    void SyncClock( iONode node );
    int GetHour();
    void stopTimer();
    iONode getLocProps();
    void showTooltip(bool p_bTooltip);

private:
    /// Creation
    bool Create();
    void init();
    wxBitmap* getIcon(const char* icon);
    void funCmd(int fidx);
    void speedCmd(bool sendCmd);
#ifdef USENEWLOOK
    bool setButtonColor( LEDButton* button, bool state );
#else
    bool setButtonColor( wxButton* button, bool state );
#endif
    /// Creates the controls and sizers
    void CreateControls();
    void setFLabels(bool init=false);

    wxPanel* m_Parent;
    iONode m_LocProps;

    wxBoxSizer* m_MainSizer;
    wxBoxSizer* m_ButtonSizer;
    wxBoxSizer* m_Button1Sizer;
    wxBoxSizer* m_Button2Sizer;
    wxBoxSizer* m_SliderSizer;
#ifdef USENEWLOOK
    LEDButton* m_FG;
    LEDButton* m_V;
    LEDButton* m_F0;
    LEDButton* m_F1;
    LEDButton* m_F2;
    LEDButton* m_F3;
    LEDButton* m_F4;
    LEDButton* m_Dir;
    Slider* m_Vslider;
    LEDButton* m_Stop;
#else
    wxButton* m_FG;
    wxTextCtrl* m_V;
    wxButton* m_F0;
    wxButton* m_F1;
    wxButton* m_F2;
    wxButton* m_F3;
    wxButton* m_F4;
    wxButton* m_Dir;
    wxSlider* m_Vslider;
    wxButton* m_Stop;
#endif

    Clock* m_Clock;
    Meter* m_Meter;

    bool m_bFn;
    bool m_bFx[32];
    bool m_bDir;
    int m_iSpeed;
    int m_iFnGroup;
};

#endif

