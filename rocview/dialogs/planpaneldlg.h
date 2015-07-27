/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifndef _PLANPANELDLG_H_
#define _PLANPANELDLG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "planpaneldlg.cpp"
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
#define ID_DIALOG_PLANPANEL_PROPS 10145
#define ID_STATICTEXT_PP_TITLE 10000
#define ID_TEXTCTRL_PP_TITLE 10001
#define ID_STATICTEXT_PP_ZLEVEL 10002
#define SYMBOL_PLANPANELPROPS_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_PLANPANELPROPS_TITLE _("PlanPanel Properties")
#define SYMBOL_PLANPANELPROPS_IDNAME ID_DIALOG_PLANPANEL_PROPS
#define SYMBOL_PLANPANELPROPS_SIZE wxDefaultSize
#define SYMBOL_PLANPANELPROPS_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

/*!
 * PlanPanelProps class declaration
 */

class PlanPanelProps: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( PlanPanelProps )
    DECLARE_EVENT_TABLE()

  void initLabels();
  void initValues();
  void evaluate();

public:
    /// Constructors
    PlanPanelProps( wxWindow* parent, iONode p_Props, int newlevel=0 );
    PlanPanelProps( );
    PlanPanelProps( wxWindow* parent, wxWindowID id = SYMBOL_PLANPANELPROPS_IDNAME, const wxString& caption = SYMBOL_PLANPANELPROPS_TITLE, const wxPoint& pos = SYMBOL_PLANPANELPROPS_POSITION, const wxSize& size = SYMBOL_PLANPANELPROPS_SIZE, long style = SYMBOL_PLANPANELPROPS_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PLANPANELPROPS_IDNAME, const wxString& caption = SYMBOL_PLANPANELPROPS_TITLE, const wxPoint& pos = SYMBOL_PLANPANELPROPS_POSITION, const wxSize& size = SYMBOL_PLANPANELPROPS_SIZE, long style = SYMBOL_PLANPANELPROPS_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin PlanPanelProps event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
    void OnHelpClick( wxCommandEvent& event );

////@end PlanPanelProps event handler declarations

////@begin PlanPanelProps member function declarations

    iONode GetProps() const { return m_Props ; }
    void SetProps(iONode value) { m_Props = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end PlanPanelProps member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin PlanPanelProps member variables
    wxStaticText* m_LabelTitle;
    wxTextCtrl* m_Title;
    wxStaticText* m_LabelZLevel;
    wxSpinCtrl* m_ZLevel;
    wxStaticText* m_labTabIdx;
    wxSpinCtrl* m_TabIdx;
    wxButton* m_OK;
    wxButton* m_Cancel;
    iONode m_Props;
////@end PlanPanelProps member variables
};

#endif
    // _PLANPANELDLG_H_
