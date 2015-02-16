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

#include "rocview/public/guiapp.h"
#include "rocview/public/base.h"

#include "rocrail/wrapper/public/Weather.h"
#include "rocrail/wrapper/public/WeatherColor.h"

#include "rocview/wrapper/public/Gui.h"

#include "rocs/public/trace.h"

ColorPanel::ColorPanel( wxWindow* parent ):wxPanel( parent, wxID_ANY, wxDefaultPosition, wxSize( 200,-1 ), wxGROW|wxFULL_REPAINT_ON_RESIZE )
{
  m_Weather = NULL;
  m_Selection = -1;
  m_Hour = -1;
  m_Min = 0;
  m_White = true;
  m_White2 = false;
  m_Brightness = true;
  m_Saturation = true;

  m_White1R     = 255;
  m_White1G     = 255;
  m_White1B     = 255;
  m_White2R     = 255;
  m_White2G     = 0;
  m_White2B     = 255;
  m_BrightnessR = 255;
  m_BrightnessG = 255;
  m_BrightnessB = 0;
  m_SaturationR = 0;
  m_SaturationG = 255;
  m_SaturationB = 255;

  this->Connect( wxEVT_PAINT, wxPaintEventHandler( ColorPanel::OnPaint ), NULL, this );
}

ColorPanel::~ColorPanel()
{
  this->Disconnect( wxEVT_PAINT, wxPaintEventHandler( ColorPanel::OnPaint ), NULL, this );
}

void ColorPanel::setDayTime(int hour, int min) {
  m_Hour = hour;
  m_Min  = min;
  Refresh();
}

void ColorPanel::setWeather(iONode weather, int selection, bool white, bool bri, bool sat, bool white2) {
  m_Weather = weather;
  m_Selection = selection;
  m_White = white;
  m_White2 = white2;
  m_Brightness = bri;
  m_Saturation = sat;
  Refresh();
}


void ColorPanel::setPen(const wxColour& color, int width, int style) {
  if( m_UseGC ) {
    wxPen pen(color);
    pen.SetWidth(width);
    pen.SetStyle(style);
    m_GC->SetPen(pen);
  }
  else {
    wxPen pen(color);
    pen.SetWidth(width);
    pen.SetStyle(style);
    m_DC->SetPen(pen);
  }
}


void ColorPanel::setBrush(const wxColour& color) {
  if( m_UseGC ) {
    wxBrush brush(color);
    m_GC->SetBrush(brush);
  }
  else {
    wxBrush brush(color);
    m_DC->SetBrush(brush);
  }
}

void ColorPanel::drawLine(int x, int y, int cx, int cy) {
  if( m_UseGC ) {
    m_GC->StrokeLine( x, y, cx, cy );
  }
  else {
    m_DC->DrawLine( x, y, cx, cy );
  }
}

void ColorPanel::drawRectangle(int x, int y, int cx, int cy) {
  if( m_UseGC ) {
    m_GC->DrawRectangle(x,y,cx,cy);
  }
  else {
    m_DC->DrawRectangle(x,y,cx,cy);
  }
}


void ColorPanel::OnPaint(wxPaintEvent& event)
{
  wxPaintDC dc(this);
  m_DC = &dc;

  dc.SetBackground(*wxBLACK_BRUSH);
  dc.Clear();

  m_UseGC = false;

  wxGraphicsContext* gc = NULL;
  if( wGui.isrendergc(wxGetApp().getIni())) {
    m_UseGC = true;
    gc = wxGraphicsContext::Create(this);
    m_GC = gc;
#ifdef wxANTIALIAS_DEFAULT
    gc->SetAntialiasMode(wxANTIALIAS_DEFAULT);
#endif
  }

  int w = 0;
  int h = 0;
  GetSize(&w, &h);

  setPen(*wxLIGHT_GREY, 1, wxDOT);
  float h10 = (float)h / 10.0;
  for( int i = 1; i < 10; i++) {
    //dc.DrawLine( 0, i*h10, w, i*h10 );
    drawLine( 0, i*h10, w, i*h10 );
  }

  int curinterval = -1;
  if( m_Hour != -1 ) {
    curinterval = (m_Hour * 60 + m_Min) / 30;
  }
  float w23 = (float)w / 23.0;
  float w47 = (float)w / 47.0;
  for( int i = 0; i < 48; i++) {
    if( curinterval != -1 && curinterval == i ) {
      setPen(wxColor( 0, 50, 0 ), 1, wxSOLID);
      setBrush( wxColor( 0, 50, 0 ));
      drawRectangle(i * w47 + 1, 0, w47, h);
    }

    if( i == m_Selection ) {
      setPen(*wxLIGHT_GREY, 3, wxDOT);
      dc.SetPen( *wxLIGHT_GREY_PEN );
      drawLine( i * w47, 0, i * w47, h );
    }
    else if( i > 0 && i < 47 ){
      setPen(*wxLIGHT_GREY, 1, wxDOT);
      drawLine( i * w47, 0, i * w47, h );
    }

  }

  TraceOp.trc( "colorpanel", TRCLEVEL_INFO, __LINE__, 9999, "width=%d height=%d", w, h );

  if( m_Weather != NULL ) {

    iONode colorProps[48] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                             NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                             NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                             NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

    iONode color = wWeather.getweathercolor(m_Weather);
    while(color != NULL) {
      int hour = wWeatherColor.gethour(color);
      int min  = wWeatherColor.getminute(color);
      if( hour < 24 && hour >= 0 && min < 60 && min >= 0) {
        colorProps[(hour*60+min)/30] = color;
      }
      color = wWeather.nextweathercolor(m_Weather, color);
    }

    TraceOp.trc( "colorpanel", TRCLEVEL_INFO, __LINE__, 9999, "check if all 30 mins are there..." );
    iONode prevColorProps = colorProps[0];
    for( int i = 0; i < 48; i++ ) {
      if( colorProps[i] == NULL && prevColorProps == NULL) {
        TraceOp.trc( "colorpanel", TRCLEVEL_INFO, __LINE__, 9999, "exit..." );
        if( gc != NULL)
          delete gc;
        return;
      }
      if( colorProps[i] == NULL && prevColorProps != NULL) {
        colorProps[i] = prevColorProps;
      }
      TraceOp.trc( "colorpanel", TRCLEVEL_INFO, __LINE__, 9999, "i=%d", i );
      prevColorProps = colorProps[i];
    }

    TraceOp.trc( "colorpanel", TRCLEVEL_INFO, __LINE__, 9999, "draw the color table..." );

    float ystep = (float)h / 255.0;

    if( m_White ) {
      setPen(wxColor( m_White1R, m_White1G, m_White1B ), 3, wxSOLID);
      int start = wWeatherColor.getwhite(colorProps[0]);
      for( int i = 1; i < 48; i++ ) {
        int val = wWeatherColor.getwhite(colorProps[i]);
        drawLine( (i-1) * w47, (255-start) * ystep, i * w47, (255-val) * ystep );
        start = val;
      }
    }

    if( m_White2 ) {
      setPen(wxColor( m_White2R, m_White2G, m_White2B ), 3, wxSOLID);
      int start = wWeatherColor.getwhite(colorProps[0]);
      for( int i = 1; i < 48; i++ ) {
        int val = wWeatherColor.getwhite2(colorProps[i]);
        drawLine( (i-1) * w47, (255-start) * ystep, i * w47, (255-val) * ystep );
        start = val;
      }
    }

    setPen(wxColor( 255, 0, 0 ), 3, wxSOLID);
    int start = wWeatherColor.getred(colorProps[0]);
    for( int i = 1; i < 48; i++ ) {
      int val = wWeatherColor.getred(colorProps[i]);
      drawLine( (i-1) * w47, (255-start) * ystep, i * w47, (255-val) * ystep );
      start = val;
    }

    setPen(wxColor( 0, 255, 0 ), 3, wxSOLID);
    start = wWeatherColor.getgreen(colorProps[0]);
    for( int i = 1; i < 48; i++ ) {
      int val = wWeatherColor.getgreen(colorProps[i]);
      drawLine( (i-1) * w47, (255-start) * ystep, i * w47, (255-val) * ystep );
      start = val;
    }

    setPen(wxColor( 0, 0, 255 ), 3, wxSOLID);
    start = wWeatherColor.getblue(colorProps[0]);
    for( int i = 1; i < 48; i++ ) {
      int val = wWeatherColor.getblue(colorProps[i]);
      drawLine( (i-1) * w47, (255-start) * ystep, i * w47, (255-val) * ystep );
      start = val;
    }

    if( m_Brightness ) {
      setPen(wxColor( m_BrightnessR, m_BrightnessG, m_BrightnessB ), 1, wxSOLID);
      start = wWeatherColor.getbri(colorProps[0]);
      for( int i = 1; i < 48; i++ ) {
        int val = wWeatherColor.getbri(colorProps[i]);
        drawLine( (i-1) * w47, (255-start) * ystep, i * w47, (255-val) * ystep );
        start = val;
      }
    }

    if( m_Saturation ) {
      setPen(wxColor( m_SaturationR, m_SaturationG, m_SaturationB ), 1, wxSOLID);
      start = wWeatherColor.getsat(colorProps[0]);
      for( int i = 1; i < 48; i++ ) {
        int val = wWeatherColor.getsat(colorProps[i]);
        drawLine( (i-1) * w47, (255-start) * ystep, i * w47, (255-val) * ystep );
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

  if( gc != NULL)
    delete gc;

}


void ColorPanel::setWhite1Color(int r, int g, int b) {
  m_White1R = r;
  m_White1G = g;
  m_White1B = b;
  Refresh();
}
void ColorPanel::setWhite2Color(int r, int g, int b) {
  m_White2R = r;
  m_White2G = g;
  m_White2B = b;
  Refresh();
}
void ColorPanel::setBrightnessColor(int r, int g, int b) {
  m_BrightnessR = r;
  m_BrightnessG = g;
  m_BrightnessB = b;
  Refresh();
}
void ColorPanel::setSaturationColor(int r, int g, int b) {
  m_SaturationR = r;
  m_SaturationG = g;
  m_SaturationB = b;
  Refresh();
}


