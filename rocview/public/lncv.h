/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifndef __ROCRAIL_rocgui_LNCV_H
#define __ROCRAIL_rocgui_LNCV_H


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "lncv.h"
#endif

#include "rocs/public/node.h"


class LNCV
{    
public:
    LNCV( wxScrolledWindow* parent, wxWindow* frame );
    void OnButton(wxCommandEvent& event);
    void event( iONode event );

private:
    bool Create();
    void CreateControls();
    void init();

    wxScrolledWindow* m_Parent;

    wxBoxSizer* m_MainBox;
    
    wxPanel* m_ItemPanel;
    wxStaticText* m_labModuleKey;
    wxTextCtrl* m_ModuleKey;
    wxButton* m_ModuleDialog;
    wxButton* m_ModuleDetect;
    wxStaticText* m_labAddress;
    wxTextCtrl* m_Address;
    wxCheckBox* m_useLNCV;
    wxTextCtrl* m_CVfrom;
    wxStaticText* m_labValue;
    wxTextCtrl* m_Value;
    wxButton* m_Start;
    wxButton* m_Stop;
    wxButton* m_Get;
    wxButton* m_Set;
};

#endif
