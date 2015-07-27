/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifndef _OUTPUTDIALOG_H_
#define _OUTPUTDIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "outputdialog.h"
#endif

/*!
 * Includes
 */
#include "basedlg.h"
#include "rocs/public/node.h"

////@begin includes
#include "wx/notebook.h"
#include "wx/listctrl.h"
#include "wx/spinctrl.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxNotebook;
class wxListCtrl;
class wxSpinCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_OUTPUT_DLG 10210
#define ID_NOTEBOOK_CO 10012
#define ID_PANEL_CO_INDEX 10005
#define ID_LISTCTRLINDEX_CO 10409
#define ID_BUTTON_CO_NEW 10007
#define ID_BUTTON_CO_DELETE 10008
#define ID_OUTPUT_DOC 10231
#define ID_PANEL_CO_GENERAL 10009
#define wxID_STATIC_CO_ID 10010
#define ID_TEXTCTRL_CO_ID 10011
#define wxID_STATIC_CO_DESC 10221
#define ID_TEXTCTRL_CO_DESC 10222
#define ID_CHECKBOX_CO_SHOW 10017
#define ID_OUTPUT_ACTIONS 10199
#define ID_PANEL_CO_INTERFACE 10021
#define wxID_STATIC_CO_IID 10022
#define ID_TEXTCTRL_CO_IID 10023
#define wxID_STATIC_CO_BUS 10260
#define ID_TEXTCTRL_CO_BUS 10259
#define wxID_STATIC_CO_PROT 10030
#define ID_CHOICE_CO_PROT 10031
#define wxID_STATIC_CO_ADDRESS 10024
#define ID_TEXTCTRL_CO_ADDRESS 10025
#define wxID_STATIC_CO_PORT 10026
#define ID_TEXTCTRL_CO_PORT 10027
#define wxID_STATIC_CO_GATE 10326
#define ID_OUTPUT_SWITCH 10199
#define ID_CO_ACCESSORY 10452
#define ID_CO_PORT_TYPE 10414
#define ID_PANEL_CO_COLOR 10006
#define ID_CO_COLOR_CHANNEL_BOX 10453
#define ID_CO_COLOR_RGB_BOX 0
#define ID_PANEL_CO_LOCATION 10013
#define wxID_STATIC_CO_X 10014
#define ID_TEXTCTRL_CO_X 10000
#define wxID_STATIC_CO_Y 10001
#define ID_TEXTCTRL_CO_Y 10002
#define wxID_STATIC_CO_Z 10003
#define ID_TEXTCTRL_CO_Z 10004
#define ID_RADIOBOX_CO_ORI 10015
#define SYMBOL_OUTPUTDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_OUTPUTDIALOG_TITLE _("OutputTable")
#define SYMBOL_OUTPUTDIALOG_IDNAME ID_OUTPUT_DLG
#define SYMBOL_OUTPUTDIALOG_SIZE wxDefaultSize
#define SYMBOL_OUTPUTDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * OutputDialog class declaration
 */

class OutputDialog: public wxDialog, public BaseDialog
{
    DECLARE_DYNAMIC_CLASS( OutputDialog )
    DECLARE_EVENT_TABLE()

  void initLabels();
  bool initIndex();
  void initValues();
  bool evaluate();
  int m_TabAlign;

public:
    /// Constructors
    OutputDialog( );
    OutputDialog( wxWindow* parent, iONode props);

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_OUTPUTDIALOG_IDNAME, const wxString& caption = SYMBOL_OUTPUTDIALOG_TITLE, const wxPoint& pos = SYMBOL_OUTPUTDIALOG_POSITION, const wxSize& size = SYMBOL_OUTPUTDIALOG_SIZE, long style = SYMBOL_OUTPUTDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();
    iONode getProperties(){ return m_Props;}
    void OnSelectPage( wxCommandEvent& event );

////@begin OutputDialog event handler declarations

    /// wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED event handler for ID_NOTEBOOK_CO
    void OnNotebookCoPageChanged( wxNotebookEvent& event );

    /// wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_LISTCTRLINDEX_CO
    void OnListctrlindexCoSelected( wxListEvent& event );

    /// wxEVT_COMMAND_LIST_COL_CLICK event handler for ID_LISTCTRLINDEX_CO
    void OnListctrlindexCoColLeftClick( wxListEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CO_NEW
    void OnButtonCoNewClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CO_DELETE
    void OnButtonCoDeleteClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_OUTPUT_DOC
    void onDoc( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_OUTPUT_ACTIONS
    void OnOutputActionsClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_OUTPUT_SWITCH
    void OnOutputSwitchClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CO_ACCESSORY
    void onAccessory( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
    void OnApplyClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
    void OnHelpClick( wxCommandEvent& event );

////@end OutputDialog event handler declarations

////@begin OutputDialog member function declarations

    iONode GetProps() const { return m_Props ; }
    void SetProps(iONode value) { m_Props = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end OutputDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin OutputDialog member variables
    wxNotebook* m_Notebook;
    wxPanel* m_IndexPanel;
    wxListCtrl* m_List2;
    wxButton* m_New;
    wxButton* m_Delete;
    wxButton* m_Doc;
    wxPanel* m_GeneralPanel;
    wxStaticText* m_LabelID;
    wxTextCtrl* m_ID;
    wxStaticText* m_LabelDescription;
    wxTextCtrl* m_Description;
    wxStaticText* m_labSVG;
    wxSpinCtrl* m_SVG;
    wxStaticText* m_labBlockID;
    wxComboBox* m_BlockID;
    wxStaticText* m_labRouteIDs;
    wxTextCtrl* m_RouteIDs;
    wxCheckBox* m_Show;
    wxCheckBox* m_TriState;
    wxRadioBox* m_Type;
    wxButton* m_Actions;
    wxPanel* m_InterfacePanel;
    wxStaticText* m_Labeliid;
    wxComboBox* m_iid;
    wxStaticText* m_Label_Bus;
    wxTextCtrl* m_Bus;
    wxStaticText* m_labUIDName;
    wxTextCtrl* m_UIDName;
    wxStaticText* m_LabelProt;
    wxChoice* m_Prot;
    wxStaticText* m_LabelAddress;
    wxTextCtrl* m_Address;
    wxStaticText* m_LabelPort;
    wxTextCtrl* m_Port;
    wxStaticText* m_labGate;
    wxRadioBox* m_Gate;
    wxStaticText* m_labGain;
    wxSpinCtrl* m_Gain;
    wxStaticText* m_labValue;
    wxSpinCtrl* m_Value;
    wxStaticText* m_labDelay;
    wxSpinCtrl* m_Delay;
    wxStaticBox* m_OptionsBox;
    wxCheckBox* m_AsSwitch;
    wxCheckBox* m_Invert;
    wxCheckBox* m_Blink;
    wxCheckBox* m_ColorType;
    wxCheckBox* m_Accessory;
    wxRadioBox* m_PortType;
    wxStaticBox* m_ColorChannelBox;
    wxStaticText* m_labRedChannel;
    wxSpinCtrl* m_RedChannel;
    wxStaticText* m_labGreenChannel;
    wxSpinCtrl* m_GreenChannel;
    wxStaticText* m_labBlueChannel;
    wxSpinCtrl* m_BlueChannel;
    wxStaticText* m_labWhiteChannel;
    wxSpinCtrl* m_WhiteChannel;
    wxStaticText* m_labWhite2Channel;
    wxSpinCtrl* m_White2Channel;
    wxStaticText* m_labBrightnessChannel;
    wxSpinCtrl* m_BrightnessChannel;
    wxStaticBox* m_ColorRGBBox;
    wxStaticText* m_labRedColor;
    wxSpinCtrl* m_RedColor;
    wxStaticText* m_labGreenColor;
    wxSpinCtrl* m_GreenColor;
    wxStaticText* m_labBlueColor;
    wxSpinCtrl* m_BlueColor;
    wxStaticText* m_labWhiteColor;
    wxSpinCtrl* m_WhiteColor;
    wxStaticText* m_labWhite2Color;
    wxSpinCtrl* m_White2Color;
    wxStaticText* m_labSaturationColor;
    wxSpinCtrl* m_SaturationColor;
    wxPanel* m_LocationPanel;
    wxStaticText* m_LabelX;
    wxTextCtrl* m_x;
    wxStaticText* m_LabelY;
    wxTextCtrl* m_y;
    wxStaticText* m_LabelZ;
    wxTextCtrl* m_z;
    wxRadioBox* m_ori;
    wxButton* m_Cancel;
    wxButton* m_OK;
    wxButton* m_Apply;
    wxButton* m_Help;
    iONode m_Props;
////@end OutputDialog member variables
};

#endif
    // _OUTPUTDIALOG_H_
