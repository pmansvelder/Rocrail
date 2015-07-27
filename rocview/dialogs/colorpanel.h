/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net




 All rights reserved.
*/

#ifndef COLORPANEL_H_
#define COLORPANEL_H_

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocs/public/node.h"
#include "rocview/public/listener.h"


class ColorPanel : public wxPanel
{
  iONode m_Weather;
  int    m_Selection;
  int    m_Hour;
  int    m_Min;
  bool   m_White;
  bool   m_White2;
  bool   m_Brightness;
  bool   m_Saturation;
  int    m_White1R;
  int    m_White1G;
  int    m_White1B;
  int    m_White2R;
  int    m_White2G;
  int    m_White2B;
  int    m_BrightnessR;
  int    m_BrightnessG;
  int    m_BrightnessB;
  int    m_SaturationR;
  int    m_SaturationG;
  int    m_SaturationB;
  bool   m_UseGC;
  wxGraphicsContext* m_GC;
  wxPaintDC* m_DC;
  wxWindow* m_Parent;
  Listener* m_Listener;

  void setPen(const wxColour& color, int width=1, int style=wxSOLID);
  void setBrush(const wxColour& color);
  void drawLine(int x, int y, int cx, int cy);
  void drawRectangle(int x, int y, int cx, int cy);

public:
  ColorPanel(wxWindow* parent);
  ~ColorPanel();
  void OnPaint(wxPaintEvent& event);
  void setWeather(iONode booster, int selection=-1, bool white=true, bool bri=true, bool sat=true, bool white2=true);
  void setDayTime(int hour, int min=0);
  void setWhite1Color(int r, int g, int b);
  void setWhite2Color(int r, int g, int b);
  void setBrightnessColor(int r, int g, int b);
  void setSaturationColor(int r, int g, int b);
  void onLeftDown( wxMouseEvent& event );
  void setListener(Listener* listener);
};

#endif /* COLORPANEL_H_ */
