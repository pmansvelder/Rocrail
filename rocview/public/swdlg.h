/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifndef __ROCRAIL_rocgui_SWITCHDLG_H
#define __ROCRAIL_rocgui_SWITCHDLG_H


// A custom modal dialog
#include <wx/spinctrl.h>

class SwCtrlDlg : public wxDialog
{
public:
  SwCtrlDlg(wxWindow *parent);

  void OnButton(wxCommandEvent& event);
  void OnSpinButton(wxSpinEvent& event);
  void OnClose(wxCloseEvent& event);

private:
  void setTooltips(void);
  int m_Bus;
  int m_Unit;

  wxButton*   m_Pin1Red;
  wxButton*   m_Pin1Green;
  wxButton*   m_Pin2Red;
  wxButton*   m_Pin2Green;
  wxButton*   m_Pin3Red;
  wxButton*   m_Pin3Green;
  wxButton*   m_Pin4Red;
  wxButton*   m_Pin4Green;

  wxStaticText* m_labIID;
  wxTextCtrl* m_IID;

  wxStaticText* m_labBus;
  wxTextCtrl* m_BusSpin;

  wxSpinCtrl* m_UnitSpin;
  wxButton*   m_Quit;

  DECLARE_EVENT_TABLE()
};

#endif
