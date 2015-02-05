/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2015 Rob Versluis, Rocrail.net

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "colorpanel.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/base.h"

#include "rocrail/wrapper/public/Weather.h"
#include "rocrail/wrapper/public/WeatherColor.h"

#include "rocs/public/trace.h"

ColorPanel::ColorPanel( wxWindow* parent ):wxPanel( parent, wxID_ANY, wxDefaultPosition, wxSize( 200,-1 ), wxGROW|wxFULL_REPAINT_ON_RESIZE )
{
  m_Weather = NULL;
  m_Selection = -1;
  m_Hour = -1;
  m_White = true;
  m_Brightness = true;
  m_Saturation = true;

  this->Connect( wxEVT_PAINT, wxPaintEventHandler( ColorPanel::OnPaint ), NULL, this );
}

ColorPanel::~ColorPanel()
{
  this->Disconnect( wxEVT_PAINT, wxPaintEventHandler( ColorPanel::OnPaint ), NULL, this );
}

void ColorPanel::setDayTime(int hour) {
  m_Hour = hour;
  Refresh();
}

void ColorPanel::setWeather(iONode weather, int selection, bool white, bool bri, bool sat) {
  m_Weather = weather;
  m_Selection = selection;
  m_White = white;
  m_Brightness = bri;
  m_Saturation = sat;
  Refresh();
}


void ColorPanel::OnPaint(wxPaintEvent& event)
{
  wxPaintDC dc(this);
  dc.SetBackground(*wxBLACK_BRUSH);
  dc.Clear();

  int w = 0;
  int h = 0;
  GetSize(&w, &h);

  dc.SetPen( *wxLIGHT_GREY_PEN );
  wxPen pen = dc.GetPen();
  pen.SetWidth(1);
  pen.SetStyle(wxDOT);
  dc.SetPen(pen);
  float h10 = (float)h / 10.0;
  for( int i = 1; i < 10; i++) {
    dc.DrawLine( 0, i*h10, w, i*h10 );
  }

  float w23 = (float)w / 23.0;
  for( int i = 0; i < 24; i++) {
    if( m_Hour != -1 && m_Hour == i ) {
      dc.SetPen( wxPen(wxColor( 0, 50, 0 )) );
      wxPen pen = dc.GetPen();
      pen.SetWidth(1);
      pen.SetStyle(wxSOLID);
      dc.SetPen(pen);
      dc.SetBrush( wxBrush(wxColor( 0, 50, 0 )));
      dc.DrawRectangle(i * w23 + 1, 0, w23, h);
    }

    if( i == m_Selection ) {
      dc.SetPen( *wxLIGHT_GREY_PEN );
      wxPen pen = dc.GetPen();
      pen.SetWidth(3);
      pen.SetStyle(wxDOT);
      dc.SetPen(pen);
      dc.DrawLine( i * w23, 0, i * w23, h );
    }
    else if( i > 0 && i < 23 ){
      dc.SetPen( *wxLIGHT_GREY_PEN );
      wxPen pen = dc.GetPen();
      pen.SetWidth(1);
      pen.SetStyle(wxDOT);
      dc.SetPen(pen);
      dc.DrawLine( i * w23, 0, i * w23, h );
    }

  }

  TraceOp.trc( "colorpanel", TRCLEVEL_INFO, __LINE__, 9999, "width=%d height=%d", w, h );

  if( m_Weather != NULL ) {

    iONode colorProps[24] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                             NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

    iONode color = wWeather.getweathercolor(m_Weather);
    while(color != NULL) {
      int hour = wWeatherColor.gethour(color);
      if( hour < 24 && hour >= 0 ) {
        colorProps[hour] = color;
      }
      color = wWeather.nextweathercolor(m_Weather, color);
    }

    for( int i = 0; i < 24; i++ ) {
      if( colorProps[i] == NULL )
        return;
    }

    wxPen pen = dc.GetPen();
    float ystep = (float)h / 255.0;

    if( m_White ) {
      dc.SetPen( *wxWHITE_PEN );
      pen = dc.GetPen();
      pen.SetWidth(3);
      dc.SetPen(pen);
      int start = wWeatherColor.getwhite(colorProps[0]);
      for( int i = 1; i < 24; i++ ) {
        int val = wWeatherColor.getwhite(colorProps[i]);
        dc.DrawLine( (i-1) * w23, (255-start) * ystep, i * w23, (255-val) * ystep );
        start = val;
      }
    }

    dc.SetPen( *wxRED_PEN );
    pen = dc.GetPen();
    pen.SetWidth(3);
    dc.SetPen(pen);
    int start = wWeatherColor.getred(colorProps[0]);
    for( int i = 1; i < 24; i++ ) {
      int val = wWeatherColor.getred(colorProps[i]);
      dc.DrawLine( (i-1) * w23, (255-start) * ystep, i * w23, (255-val) * ystep );
      start = val;
    }

    dc.SetPen( *wxGREEN_PEN );
    pen = dc.GetPen();
    pen.SetWidth(3);
    dc.SetPen(pen);
    start = wWeatherColor.getgreen(colorProps[0]);
    for( int i = 1; i < 24; i++ ) {
      int val = wWeatherColor.getgreen(colorProps[i]);
      dc.DrawLine( (i-1) * w23, (255-start) * ystep, i * w23, (255-val) * ystep );
      start = val;
    }

    dc.SetPen( wxColour(0,0,255) );
    pen = dc.GetPen();
    pen.SetWidth(3);
    dc.SetPen(pen);
    start = wWeatherColor.getblue(colorProps[0]);
    for( int i = 1; i < 24; i++ ) {
      int val = wWeatherColor.getblue(colorProps[i]);
      dc.DrawLine( (i-1) * w23, (255-start) * ystep, i * w23, (255-val) * ystep );
      start = val;
    }

    if( m_Brightness ) {
      dc.SetPen( wxColour(255,255,0) );
      pen = dc.GetPen();
      pen.SetWidth(1);
      dc.SetPen(pen);
      start = wWeatherColor.getbri(colorProps[0]);
      for( int i = 1; i < 24; i++ ) {
        int val = wWeatherColor.getbri(colorProps[i]);
        dc.DrawLine( (i-1) * w23, (255-start) * ystep, i * w23, (255-val) * ystep );
        start = val;
      }
    }

    if( m_Saturation ) {
      dc.SetPen( wxColour(0,255,255) );
      pen = dc.GetPen();
      pen.SetWidth(1);
      dc.SetPen(pen);
      start = wWeatherColor.getsat(colorProps[0]);
      for( int i = 1; i < 24; i++ ) {
        int val = wWeatherColor.getsat(colorProps[i]);
        dc.DrawLine( (i-1) * w23, (255-start) * ystep, i * w23, (255-val) * ystep );
        start = val;
      }
    }

  }
  else {
    TraceOp.trc( "colorpanel", TRCLEVEL_INFO, __LINE__, 9999, "No weather selected..." );
    dc.SetTextBackground(*wxBLACK);
    dc.SetTextForeground(*wxWHITE);
    dc.DrawText(wxT("No weather selected..."), 10, h/2);
  }

}
