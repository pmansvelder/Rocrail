/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 All rights reserved.
*/

#ifndef _SCHEDULEGRAPH_H_
#define _SCHEDULEGRAPH_H_

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocs/public/node.h"
#include "rocs/public/map.h"



class ScheduleGraph: public wxPanel
{
  iONode m_Schedule;
  iONode getEntry( iOMap map, iONode scentry );
public:
    /// Constructors
    ScheduleGraph(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSUNKEN_BORDER|wxTAB_TRAVERSAL);
    ~ScheduleGraph();
    void OnPaint(wxPaintEvent& event);
    void setSchedule(iONode schedule);
};

#endif
    // _SCHEDULEGRAPH_H_
