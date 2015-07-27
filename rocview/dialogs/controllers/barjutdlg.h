/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef _BARJUTDLG_H_
#define _BARJUTDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "barjutdlg.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/spinctrl.h"
////@end includes
#include "rocs/public/node.h"


/*!
 * Forward declarations
 */

////@begin forward declarations
class wxSpinCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_BARJUT_SETTINGS 10034
#define ID_STATICTEXT_BARJUT_IID 10000
#define ID_TEXTCTRL_BARJUT_IID 10001
#define ID_STATICTEXT_BARJUT_DEVICE 10002
#define ID_STATICTEXT_BARJUT_POLLING 10004
#define SYMBOL_BARJUTCNTRLDLG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_BARJUTCNTRLDLG_TITLE _("BarJuT")
#define SYMBOL_BARJUTCNTRLDLG_IDNAME ID_DIALOG_BARJUT_SETTINGS
#define SYMBOL_BARJUTCNTRLDLG_SIZE wxDefaultSize
#define SYMBOL_BARJUTCNTRLDLG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * BarJuTCntrlDlg class declaration
 */

class BarJuTCntrlDlg: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( BarJuTCntrlDlg )
    DECLARE_EVENT_TABLE()
  void initLabels();
  void initValues();
  void evaluate();
  iONode m_Props;
  const char* m_Devices;

public:
    /// Constructors
    BarJuTCntrlDlg( );
    BarJuTCntrlDlg( wxWindow* parent, iONode props, const char* devices=NULL );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_BARJUTCNTRLDLG_IDNAME, const wxString& caption = SYMBOL_BARJUTCNTRLDLG_TITLE, const wxPoint& pos = SYMBOL_BARJUTCNTRLDLG_POSITION, const wxSize& size = SYMBOL_BARJUTCNTRLDLG_SIZE, long style = SYMBOL_BARJUTCNTRLDLG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin BarJuTCntrlDlg event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOKClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCANCELClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
    void OnHelpClick( wxCommandEvent& event );

////@end BarJuTCntrlDlg event handler declarations

////@begin BarJuTCntrlDlg member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end BarJuTCntrlDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin BarJuTCntrlDlg member variables
    wxStaticText* m_labIID;
    wxTextCtrl* m_IID;
    wxStaticText* m_labDevice;
    wxComboBox* m_Device;
    wxStaticText* m_labPolling;
    wxSpinCtrl* m_Polling;
////@end BarJuTCntrlDlg member variables
};

#endif
    // _BARJUTDLG_H_
