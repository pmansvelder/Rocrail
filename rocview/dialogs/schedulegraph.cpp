/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 All rights reserved.
*/


#include "schedulegraph.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocrail/wrapper/public/Schedule.h"
#include "rocrail/wrapper/public/ScheduleEntry.h"

#include "rocs/public/trace.h"
#include "rocs/public/map.h"

ScheduleGraph::ScheduleGraph(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
  wxPanel::Create(parent, id, pos, size, style);
  m_Schedule = NULL;
  this->Connect( wxEVT_PAINT, wxPaintEventHandler( ScheduleGraph::OnPaint ), NULL, this );
}


ScheduleGraph::~ScheduleGraph()
{
  this->Disconnect( wxEVT_PAINT, wxPaintEventHandler( ScheduleGraph::OnPaint ), NULL, this );
}


iONode ScheduleGraph::getEntry( iOMap map, iONode scentry ) {
  const char* location = wScheduleEntry.getlocation(scentry);
  const char* block = wScheduleEntry.getblock(scentry);
  if( location != NULL && StrOp.len(location) > 0 ) {
    if( MapOp.haskey(map, location) ) {
      return (iONode)MapOp.get(map, location);
    }
  }
  else if( block != NULL && StrOp.len(block) > 0 ) {
    if( MapOp.haskey(map, block) ) {
      return (iONode)MapOp.get(map, block);
    }
  }
  return NULL;
}

void ScheduleGraph::OnPaint(wxPaintEvent& event)
{
  wxPaintDC dc(this);
  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  if( m_Schedule == NULL )
    return;

  int w = 0;
  int h = 0;
  GetSize(&w, &h);

  int nrentries = 0;
  int nrtime = 0;
  iOMap  map  = MapOp.inst();
  iOList list = ListOp.inst();
  iONode scentry = wSchedule.getscentry( m_Schedule );
  while( scentry != NULL ) {
    const char* location = wScheduleEntry.getlocation(scentry);
    const char* block = wScheduleEntry.getblock(scentry);
    if( location != NULL && StrOp.len(location) > 0 ) {
      if( !MapOp.haskey(map, location) ) {
        MapOp.put(map, location, (obj)scentry);
        ListOp.add(list, (obj)scentry);
        nrentries++;
      }
    }
    else if( block != NULL && StrOp.len(block) > 0 ) {
      if( !MapOp.haskey(map, block) ) {
        MapOp.put(map, block, (obj)scentry);
        ListOp.add(list, (obj)scentry);
        nrentries++;
      }
    }
    nrtime++;
    scentry = wSchedule.nextscentry( m_Schedule, scentry );
  };


  if( nrentries > 1 ) {
    dc.SetPen( *wxGREY_PEN );
    wxPen pen = dc.GetPen();
    pen.SetWidth(1);
    dc.SetPen(pen);

    wxCoord tw;
    wxCoord th;
    wxFont* font = new wxFont(dc.GetFont());
    dc.GetTextExtent(wxString("M",wxConvUTF8), &tw, &th, 0,0, font);
    delete font;
    float twidth  = tw;
    float theight = th;


    float margin = 10.0;
    float leftmargin = 40.0;
    float topmargin = 80.0;
    float v = (float)((float)(w-2*margin-leftmargin) / (float)(nrentries-1));

    // Vertical destination lines:
    for( int i = 0; i < nrentries; i++) {
      iONode scentry = (iONode)ListOp.get(list, i);
      const char* location = wScheduleEntry.getlocation(scentry);
      const char* block = wScheduleEntry.getblock(scentry);
      int x = i*v+leftmargin+margin;
      NodeOp.setInt( scentry, "gx", x);

      TraceOp.trc( "scgraph", TRCLEVEL_INFO, __LINE__, 9999, "draw line[%d of %d] at x=%d for [%s][%s]",
          i, nrentries, x, location != NULL ? location:"-", block != NULL ? block:"-");

      dc.DrawLine( x, topmargin, x, h );
      if( location != NULL && StrOp.len(location) > 0 )
        dc.DrawRotatedText( wxString(location,wxConvUTF8), x-(theight/2), topmargin, 90 );
      else
        dc.DrawRotatedText( wxString(block,wxConvUTF8), x-(theight/2), topmargin, 90 );
    }

    // Horizontal time lines:
    scentry = wSchedule.getscentry( m_Schedule );
    float timeh = (h - topmargin - 2*margin) / (nrtime-1);
    for( int n = 0; n < nrtime; n++) {
      int y = topmargin + margin + timeh*n;
      dc.DrawLine( leftmargin, y, w, y );
      if( wScheduleEntry.isregularstop(scentry) )
        dc.DrawRotatedText( wxString::Format(wxT("%02d:%02d"),wScheduleEntry.gethour(scentry),wScheduleEntry.getminute(scentry)), margin/2, y - (theight/2), 0 );
      NodeOp.setInt( scentry, "gy", y);
      scentry = wSchedule.nextscentry( m_Schedule, scentry );
    }

    // The graphic:
    dc.SetPen( *wxBLACK_PEN );
    dc.SetBrush( *wxBLACK_BRUSH );
    wxPen pen2 = dc.GetPen();
    pen2.SetWidth(3);
    dc.SetPen(pen2);

    scentry = wSchedule.getscentry( m_Schedule );
    int gx = -1;
    int gy = -1;
    int idx = 0;
    while( scentry != NULL ) {
      int x = NodeOp.getInt( scentry, "gx", 0);
      int y = NodeOp.getInt( scentry, "gy", 0);
      if( gx == -1 ) {
        // The first entry:
        gx = x;
        gy = y;
      }
      else {
        iONode node = getEntry(map, scentry);
        if( node != NULL )
          x = NodeOp.getInt( node, "gx", 0);
        TraceOp.trc( "scgraph", TRCLEVEL_INFO, __LINE__, 9999, "draw line[%d] from %d,%d to %d,%d", idx, gx,gy,x,y );
        dc.DrawLine( gx, gy, x, y );
        gx = x;
        gy = y;
      }
      if( wScheduleEntry.isregularstop(scentry) )
        dc.DrawCircle(gx, gy, 5);
      else
        dc.DrawRectangle(gx-3, gy-3, 6, 6);
      scentry = wSchedule.nextscentry( m_Schedule, scentry );
      idx++;
    }

  }

  MapOp.base.del(map);
  ListOp.base.del(list);
}

void ScheduleGraph::setSchedule(iONode schedule) {
  TraceOp.trc( "scgraph", TRCLEVEL_INFO, __LINE__, 9999, "Set Schedule: %s", wSchedule.getid(schedule) );
  m_Schedule = schedule;
  Refresh();
}

