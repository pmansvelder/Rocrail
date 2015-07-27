/*
 Rocrail - Model Railroad Software
 Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net
 All rights reserved.
*/
#ifndef __locowidget__
#define __locowidget__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/event.h"
#endif

#include "locowidgetgen.h"
#include "rocs/public/node.h"

//// end generated include

/** Implementing LocoWidgetGen */
class LocoWidget : public LocoWidgetGen
{
  iONode m_Props;
  bool m_hasImageRequested;
  char m_Pixpath[256];
  wxBitmap scale(wxBitmap bmp, int max_width, int max_height);
  void InitLoco();

  void onMenuStart( wxCommandEvent& event );
  void onMenuStop( wxCommandEvent& event );
  void onMenuSoftReset( wxCommandEvent& event );
  void onMenuResetAll( wxCommandEvent& event );
  void onMenuSwap( wxCommandEvent& event );
  void onMenuSwapBlockSide( wxCommandEvent& event );
  void onMenuGoTo( wxCommandEvent& event );
  void onMenuSchedule( wxCommandEvent& event );
  void onMenuTour( wxCommandEvent& event );
  void onMenuProps( wxCommandEvent& event );

  void onThrottle( wxMouseEvent& event );
  void onPopUp( wxMouseEvent& event );
  void onStop( wxCommandEvent& event );
  void onImageLeftDown( wxMouseEvent& event );
  void onSpeedUp( wxMouseEvent& event );
  void onSpeedDown( wxMouseEvent& event );
  void onMouseWheel( wxMouseEvent& event );
  void onDirection( wxMouseEvent& event );
  void onDestination( wxMouseEvent& event );
  void onModeStart( wxMouseEvent& event );
  void onModeStop( wxMouseEvent& event );

public:
  /** Constructor */
  LocoWidget( wxWindow* parent, iONode props, int width );
  void UpdateLocoImg();
  void UpdateLoco(iONode props);
	//// end generated class members
	
};

#endif // __locowidget__
