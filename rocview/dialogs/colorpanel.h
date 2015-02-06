/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net




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

#ifndef COLORPANEL_H_
#define COLORPANEL_H_

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocs/public/node.h"


class ColorPanel : public wxPanel
{
  iONode m_Weather;
  int    m_Selection;
  int    m_Hour;
  bool   m_White;
  bool   m_Brightness;
  bool   m_Saturation;
  bool   m_UseGC;
  wxGraphicsContext* m_GC;
  wxPaintDC* m_DC;
  void setPen(const wxColour& color, int width=1, int style=wxSOLID);
  void setBrush(const wxColour& color);
  void drawLine(int x, int y, int cx, int cy);
  void drawRectangle(int x, int y, int cx, int cy);

public:
  ColorPanel(wxWindow* parent);
  ~ColorPanel();
  void OnPaint(wxPaintEvent& event);
  void setWeather(iONode booster, int selection=-1, bool white=true, bool bri=true, bool sat=true);
  void setDayTime(int hour);
};

#endif /* COLORPANEL_H_ */
