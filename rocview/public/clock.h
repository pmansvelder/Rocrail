/*
 Rocrail - Model Railroad Software
 Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net
 All rights reserved.
*/

#ifndef __Clock_H__
#define __Clock_H__

#define ID_WXTIMER	999
#define TIMER		1000

#include <wx/datetime.h>
#include <wx/image.h>
#include <wx/math.h>
#include <wx/timer.h>
#include <wx/panel.h>

class Clock : public wxPanel{
	public:
    Clock(wxWindow *parent, wxWindowID id, int x, int y, int handwidth, int devider=1, int clocktype=0, bool showsecondhand=true);

    void OnPaint(wxPaintEvent& event);
    void Timer(wxTimerEvent& event);
    void SetDevider(int devider);
    void SetTime(long rocrailtime);
    long GetTime();
    int GetHour();
    void OnPopup(wxMouseEvent& event);
    void OnAdjustTime(wxCommandEvent& event);
    void OnFreezeTime(wxCommandEvent& event);
    void OnResumeTime(wxCommandEvent& event);
    void OnHelp(wxCommandEvent& event);
    void SyncClock( iONode node );
    void stopTimer();
    void calculate();
    void drawClock();
    void drawNewClock();
    void drawSecondHand(wxGraphicsContext* gc, double c, bool erase=false);

	private:
    wxBitmap* m_Logo;
    wxWindow* m_Parent;
		wxTimer *WxTimer;
		double x,y,z;
		bool start;
    wxDateTime* datetime;
    time_t ltime;
    int devider;
    int m_Temp;
    bool deviderchanged;
    bool run;
    bool showsecondhand;
    int type;
    int hours, minutes;
    bool second;

		DECLARE_EVENT_TABLE()
};

enum {
  ME_AdjustTime = 201,
  ME_FreezeTime,
  ME_ResumeTime,
  ME_ClockHelp,
};

#endif

