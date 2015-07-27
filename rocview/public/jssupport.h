/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifndef __ROCRAIL_rocgui_JSSUPPORT_H
#define __ROCRAIL_rocgui_JSSUPPORT_H

#include "rocs/public/rocs.h"
#include "rocs/public/doc.h"
#include "rocs/public/node.h"
#include "rocs/public/trace.h"
#include "rocs/public/thread.h"
#include "rocview/dialogs/locseldlg.h"


class JsSupport : public wxEvtHandler
{
public:
  static JsSupport* inst( iONode ini );
  bool isRunning() { return m_bRun; }
  iONode getIni() { return m_Ini; }
  int getDev4ID( const char* locID);

  bool m_bUp[4];
  bool m_bDown[4];
  unsigned long m_UpTime[4];
  unsigned long m_DownTime[4];
  iONode m_Selected[4];

private:
  JsSupport( iONode ini );
  void speedCmd( int device );
  void funCmd( int device, int fnchanged );
  LocSelDlg* m_LocDialog[4];
  bool m_bF1[4];
  bool m_bF2[4];
  bool m_bF3[4];
  bool m_bF4[4];
  bool m_bF5[4];
  bool m_bF6[4];
  bool m_bF7[4];
  bool m_bF8[4];
  bool m_bF9[4];
  bool m_bF10[4];
  bool m_bF11[4];
  bool m_bF12[4];
  iONode m_Ini;
  void initJS();
  void OnJsEvent(wxCommandEvent& event);

  bool m_bRun;
  iOThread repeater;

  // any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
};

enum
{
  JSSUPPORT_EVENT=1
};

#endif
