/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifndef __ROCRAIL_rocgui_STATBAR_H
#define __ROCRAIL_rocgui_STATBAR_H

/*!
 * Includes
 */

#include "wx/statusbr.h"
#include "wx/statbmp.h"

class StatusBar : public wxStatusBar
{
public:
    StatusBar(wxWindow *parent);
    virtual ~StatusBar();

    void OnSize(wxSizeEvent& event);
    void Update(int flags);
    void Health(bool health);
    void Automode(bool automode);

private:

    wxStaticBitmap *m_statbmpH;
    wxStaticBitmap *m_statbmpP;
    wxStaticBitmap *m_statbmpT;
    wxStaticBitmap *m_statbmpS;
    wxStaticBitmap *m_statbmpA;
    wxStaticBitmap *m_statbmpR;

    DECLARE_EVENT_TABLE()
};

enum {
  status_info = 0,
  status_rcon,
  status_digint,
  status_load,
  status_controller,
  status_max
};

#define BITMAP_SIZE_X 16
#define BITMAP_SIZE_Y 16

#endif
