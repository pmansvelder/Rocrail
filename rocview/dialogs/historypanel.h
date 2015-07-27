/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef HISTORYPANEL_H_
#define HISTORYPANEL_H_

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocs/public/node.h"


class HistoryPanel : public wxPanel
{
  iONode m_Booster;
public:
  HistoryPanel(wxWindow* parent);
  ~HistoryPanel();
  void OnPaint(wxPaintEvent& event);
  void setBooster(iONode booster);
};

#endif /* HISTORYPANEL_H_ */
