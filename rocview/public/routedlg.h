/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifndef __ROCRAIL_rocgui_STREETDLG_H
#define __ROCRAIL_rocgui_STREETDLG_H


// A custom modal dialog
#include <wx/grid.h>

class RouteCtrlDlg : public wxDialog
{
public:
  RouteCtrlDlg(wxWindow *parent);

  void OnButton(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);

private:
  void init();
  void fillTable(iONode node);
  wxGrid* m_Grid;

  wxButton*   m_Quit;
  wxButton*   m_Test;
  wxButton*   m_Force;
  wxButton*   m_Lock;

  DECLARE_EVENT_TABLE()
};

#endif
