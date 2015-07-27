/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/


#ifndef __dipdlg__
#define __dipdlg__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/spinctrl.h>
#include <wx/sizer.h>
#include <wx/radiobox.h>
#include <wx/button.h>
#include <wx/dialog.h>

#include <rocs/public/node.h>

class DIPDlg : public wxDialog
{
  private:
  const char* m_Title;
  iONode m_DIP;
  int m_CVNr;
  int m_Value;
  void initDIP();
  void addDIPGroup(iONode group, int idx);
  wxRadioBox* m_RadioBox[32];
  wxCheckBox* m_CheckBox[32][32];
  iONode m_Group[32][32];
  wxWindow* m_Parent;

  protected:
    wxStdDialogButtonSizer* m_sdButtons;
    wxButton* m_sdButtonsOK;
    wxButton* m_sdButtonsApply;
    wxButton* m_sdButtonsCancel;
    wxBoxSizer* m_Sizer;


    void onCancel( wxCommandEvent& event );
    void onOK( wxCommandEvent& event );
    void onApply( wxCommandEvent& event );
    void onClose( wxCloseEvent& event );



  public:

    DIPDlg( wxWindow* parent, iONode dip, int cvnr, int value, const char* title=NULL );
    ~DIPDlg();

    int getValue(int* cvnr);

};

#endif //__fxdlggen__
