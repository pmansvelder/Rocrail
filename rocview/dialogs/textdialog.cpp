/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "textdialog.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
#include "wx/imaglist.h"
////@end includes
#include <wx/colordlg.h>

#include "textdialog.h"
#include "actionsctrldlg.h"

#include "rocrail/wrapper/public/Text.h"
#include "rocrail/wrapper/public/TextList.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocview/wrapper/public/Gui.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocview/public/guiapp.h"
#include "rocs/public/system.h"

////@begin XPM images
////@end XPM images

/*!
 * TextDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( TextDialog, wxDialog )

/*!
 * TextDialog event table definition
 */

BEGIN_EVENT_TABLE( TextDialog, wxDialog )

////@begin TextDialog event table entries
    EVT_LIST_ITEM_SELECTED( ID_INDEXLIST, TextDialog::OnIndexlistSelected )
    EVT_BUTTON( ID_NEWTEXT, TextDialog::OnNewtextClick )
    EVT_BUTTON( ID_DELETETEXT, TextDialog::OnDeletetextClick )
    EVT_BUTTON( ID_BUTTON_TXT_IMAGE, TextDialog::OnButtonTxtImageClick )
    EVT_BUTTON( ID_BUTTON_TEXT_COLOR, TextDialog::OnButtonTextColorClick )
    EVT_BUTTON( ID_BUTTON_TEXT_BACKGROUND, TextDialog::OnButtonTextBackgroundClick )
    EVT_BUTTON( ID_TEXT_ACTIONS, TextDialog::OnTextActionsClick )
    EVT_BUTTON( wxID_OK, TextDialog::OnOkClick )
    EVT_BUTTON( wxID_CANCEL, TextDialog::OnCancelClick )
    EVT_BUTTON( wxID_HELP, TextDialog::OnHelpClick )
    EVT_BUTTON( wxID_APPLY, TextDialog::OnApplyClick )
////@end TextDialog event table entries

END_EVENT_TABLE()

/*!
 * TextDialog constructors
 */

TextDialog::TextDialog( )
{
}

TextDialog::TextDialog( wxWindow* parent, iONode p_Props )
{
  m_TabAlign = wxGetApp().getTabAlign();
  Create(parent, -1, wxGetApp().getMsg("text") );
  m_Props = p_Props;
  initLabels();
  initValues();

  m_IndexPanel->GetSizer()->Layout();
  m_GeneralPanel->GetSizer()->Layout();
  m_LocationPanel->GetSizer()->Layout();
  m_InterfacePanel->GetSizer()->Layout();
  m_Notebook->Fit();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  if( initIndex() ) {
    initValues();
    m_Notebook->SetSelection( 1 );
  }

}

void TextDialog::initLabels() {
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "general" ) );
  m_Notebook->SetPageText( 2, wxGetApp().getMsg( "interface" ) );
  m_Notebook->SetPageText( 3, wxGetApp().getMsg( "position" ) );

  // Index
  initList(m_IndexList, this);
  m_New->SetLabel( wxGetApp().getMsg( "new" ) );
  m_Delete->SetLabel( wxGetApp().getMsg( "delete" ) );

  // General
  m_LabelID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_LabelText->SetLabel( wxGetApp().getMsg( "text" ) );
  m_LabelTip->SetLabel( wxGetApp().getMsg( "tip" ) );
  m_labPointsize->SetLabel( wxGetApp().getMsg( "pointsize" ) );
  m_labColor->SetLabel( wxGetApp().getMsg( "color" ) );
  m_labBackColor->SetLabel( wxGetApp().getMsg( "background" ) );
  m_AttributesBox->SetLabel( wxGetApp().getMsg( "attributes" ) );
  m_labImage->SetLabel( wxGetApp().getMsg( "image" ) );
  m_Bold->SetLabel(wxGetApp().getMsg( "bold" ) );
  m_Italic->SetLabel(wxGetApp().getMsg( "italic" ) );
  m_Underlined->SetLabel(wxGetApp().getMsg( "underlined" ) );
  m_Border->SetLabel(wxGetApp().getMsg( "border" ) );
  m_Transparent->SetLabel(wxGetApp().getMsg( "transparent" ) );
  m_Clock->SetLabel( wxGetApp().getMsg( "fastclock" ) );
  m_Actions->SetLabel( wxGetApp().getMsg( "actions" )+_T("...") );
  m_Reset->SetLabel(wxGetApp().getMsg( "reset" ) );
  m_Input->SetLabel(wxGetApp().getMsg( "inputfield" ) );

  m_Ori->SetLabel(wxGetApp().getMsg( "orientation" ) );
  m_Ori->SetString( 0, wxGetApp().getMsg( "default" ) );
  m_Ori->SetString( 1, wxGetApp().getMsg( "up" ) );
  m_Ori->SetString( 2, wxGetApp().getMsg( "down" ) );

  // Interface
  BaseDialog::addIIDs(m_IID);

  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_labBus->SetLabel( wxGetApp().getMsg( "bus" ) );
  m_labAddress->SetLabel( wxGetApp().getMsg( "address" ) );
  m_labDisplay->SetLabel( wxGetApp().getMsg( "display" ) );


  // Location
  m_LabelX->SetLabel( wxGetApp().getMsg( "x" ) );
  m_LabelY->SetLabel( wxGetApp().getMsg( "y" ) );
  m_LabelZ->SetLabel( wxGetApp().getMsg( "z" ) );
  m_LabelCx->SetLabel( wxGetApp().getMsg( "cx" ) );
  m_LabelCy->SetLabel( wxGetApp().getMsg( "cy" ) );

  // Buttons
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  m_Apply->SetLabel( wxGetApp().getMsg( "apply" ) );
  m_Help->SetLabel( wxGetApp().getMsg( "help" ) );
}

bool TextDialog::initIndex() {
  TraceOp.trc( "textdlg", TRCLEVEL_INFO, __LINE__, 9999, "InitIndex" );
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode txlist = wPlan.gettxlist( model );
    if( txlist != NULL ) {
      fillIndex(txlist);

      if( m_Props != NULL ) {
        setIDSelection(wItem.getid( m_Props ));
        return true;
      }
      else {
        m_Props = setSelection(0);
      }
    }
  }
  return false;
}



void TextDialog::initValues() {
  char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("text").mb_str(wxConvUTF8), wText.getid( m_Props ) );
  SetTitle( wxString(title,wxConvUTF8) );
  StrOp.free( title );

  // General
  m_ID->SetValue( wxString(wText.getid( m_Props ),wxConvUTF8) );
  m_Text->SetValue( wxString(wText.gettext( m_Props ),wxConvUTF8) );
  m_Tip->SetValue( wxString(wText.getdesc( m_Props ),wxConvUTF8) );
  char* str = StrOp.fmt( "%d", wText.getpointsize( m_Props ) );
  m_Pointsize->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );


  m_Bold->SetValue( wText.isbold( m_Props) );
  m_Underlined->SetValue( wText.isunderlined( m_Props) );
  m_Border->SetValue( wText.isborder( m_Props) );
  m_Italic->SetValue( wText.isitalic( m_Props ) );
  m_Transparent->SetValue( wText.istransparent( m_Props ) );
  m_Input->SetValue( wText.ismanualinput( m_Props ) );
  m_Reset->SetValue( wText.isreset( m_Props ) );
  m_Clock->SetValue(wText.isclock( m_Props )?true:false);

  wxColour color( wText.getred(m_Props), wText.getgreen(m_Props), wText.getblue(m_Props) );
  m_Color->SetBackgroundColour(color);
  wxColour bcolor( wText.getbackred(m_Props), wText.getbackgreen(m_Props), wText.getbackblue(m_Props) );
  m_Background->SetBackgroundColour(bcolor);

  if( StrOp.equals( wText.getori(m_Props), wItem.north ) )
    m_Ori->SetSelection(1);
  else if( StrOp.equals( wText.getori(m_Props), wItem.south ) )
    m_Ori->SetSelection(2);
  else
    m_Ori->SetSelection(0);

  // Interface
  m_IID->SetValue( wxString(wText.getiid( m_Props ),wxConvUTF8) );
  m_Bus->SetValue( wxString::Format(wxT("%d"), wText.getbus( m_Props )) );
  m_Address->SetValue( wText.getaddr( m_Props ) );
  m_Display->SetValue( wText.getdisplay( m_Props ) );

  // Location
  str = StrOp.fmt( "%d", wText.getx( m_Props ) );
  m_x->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wText.gety( m_Props ) );
  m_y->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wText.getz( m_Props ) );
  m_z->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wText.getcx( m_Props ) );
  m_Cx->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wText.getcy( m_Props ) );
  m_Cy->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );

}


bool TextDialog::evaluate() {
  if( m_Props == NULL )
    return false;

  iONode model  = wxGetApp().getModel();
  iONode txlist = wPlan.gettxlist( model );

  if( existID( this, txlist, m_Props, m_ID->GetValue() ) ) {
    m_ID->SetValue( wxString(wItem.getid( m_Props ),wxConvUTF8) );
    return false;
  }

  // General
  wItem.setprev_id( m_Props, wItem.getid(m_Props) );
  wText.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );
  wText.settext( m_Props, m_Text->GetValue().mb_str(wxConvUTF8) );
  wText.setdesc( m_Props, m_Tip->GetValue().mb_str(wxConvUTF8) );
  wText.setpointsize( m_Props, atoi( m_Pointsize->GetValue().mb_str(wxConvUTF8) ) );

  wText.setbold( m_Props, m_Bold->IsChecked()?True:False );
  wText.setunderlined( m_Props, m_Underlined->IsChecked()?True:False );
  wText.setborder( m_Props, m_Border->IsChecked()?True:False );
  wText.setitalic( m_Props, m_Italic->IsChecked()?True:False );
  wText.settransparent( m_Props, m_Transparent->IsChecked()?True:False );
  wText.setmanualinput( m_Props, m_Input->IsChecked()?True:False );
  wText.setreset( m_Props, m_Reset->IsChecked()?True:False );
  wText.setclock( m_Props, m_Clock->IsChecked() ? True:False );

  if( m_Ori->GetSelection() == 0 )
    wText.setori(m_Props, wItem.west );
  else if( m_Ori->GetSelection() == 1 )
    wText.setori(m_Props, wItem.north );
  else if( m_Ori->GetSelection() == 2 )
    wText.setori(m_Props, wItem.south );

  // Interface
  wText.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );
  wText.setbus( m_Props, atoi(m_Bus->GetValue().mb_str(wxConvUTF8)) );
  wText.setaddr( m_Props, m_Address->GetValue() );
  wText.setdisplay( m_Props, m_Display->GetValue() );

  // Location
  wText.setx( m_Props, atoi( m_x->GetValue().mb_str(wxConvUTF8) ) );
  wText.sety( m_Props, atoi( m_y->GetValue().mb_str(wxConvUTF8) ) );
  wText.setz( m_Props, atoi( m_z->GetValue().mb_str(wxConvUTF8) ) );
  wText.setcx( m_Props, atoi( m_Cx->GetValue().mb_str(wxConvUTF8) ) );
  wText.setcy( m_Props, atoi( m_Cy->GetValue().mb_str(wxConvUTF8) ) );

  return true;
}


/*!
 * TextDialog creator
 */

bool TextDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin TextDialog member initialisation
    m_Notebook = NULL;
    m_IndexPanel = NULL;
    m_IndexList = NULL;
    m_New = NULL;
    m_Delete = NULL;
    m_GeneralPanel = NULL;
    m_LabelID = NULL;
    m_ID = NULL;
    m_LabelText = NULL;
    m_Text = NULL;
    m_labImage = NULL;
    m_ImageButton = NULL;
    m_LabelTip = NULL;
    m_Tip = NULL;
    m_labPointsize = NULL;
    m_Pointsize = NULL;
    m_labColor = NULL;
    m_Color = NULL;
    m_labBackColor = NULL;
    m_Background = NULL;
    m_AttributesBox = NULL;
    m_Bold = NULL;
    m_Underlined = NULL;
    m_Italic = NULL;
    m_Transparent = NULL;
    m_Input = NULL;
    m_Border = NULL;
    m_Clock = NULL;
    m_Ori = NULL;
    m_Reset = NULL;
    m_Actions = NULL;
    m_InterfacePanel = NULL;
    m_labIID = NULL;
    m_IID = NULL;
    m_labBus = NULL;
    m_Bus = NULL;
    m_labAddress = NULL;
    m_Address = NULL;
    m_labDisplay = NULL;
    m_Display = NULL;
    m_LocationPanel = NULL;
    m_LabelX = NULL;
    m_x = NULL;
    m_LabelY = NULL;
    m_y = NULL;
    m_LabelZ = NULL;
    m_z = NULL;
    m_LabelCx = NULL;
    m_Cx = NULL;
    m_LabelCy = NULL;
    m_Cy = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
    m_Help = NULL;
    m_Apply = NULL;
////@end TextDialog member initialisation

////@begin TextDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end TextDialog creation
    return true;
}

/*!
 * Control creation for TextDialog
 */

void TextDialog::CreateControls()
{
////@begin TextDialog content construction
    TextDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_Notebook = new wxNotebook( itemDialog1, ID_NOTEBOOK_TEXT, wxDefaultPosition, wxDefaultSize, m_TabAlign );

    m_IndexPanel = new wxPanel( m_Notebook, ID_INDEX_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    m_IndexPanel->SetSizer(itemBoxSizer5);

    m_IndexList = new wxListCtrl( m_IndexPanel, ID_INDEXLIST, wxDefaultPosition, wxSize(100, 100), wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES );
    itemBoxSizer5->Add(m_IndexList, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer5->Add(itemBoxSizer7, 0, wxALIGN_LEFT, 5);
    m_New = new wxButton( m_IndexPanel, ID_NEWTEXT, _("New"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer7->Add(m_New, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Delete = new wxButton( m_IndexPanel, ID_DELETETEXT, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer7->Add(m_Delete, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_IndexPanel, _("Index"));

    m_GeneralPanel = new wxPanel( m_Notebook, ID_GEN_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxVERTICAL);
    m_GeneralPanel->SetSizer(itemBoxSizer11);

    wxFlexGridSizer* itemFlexGridSizer12 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer11->Add(itemFlexGridSizer12, 0, wxGROW|wxALL, 5);
    m_LabelID = new wxStaticText( m_GeneralPanel, ID_STATICTEXT_TEXT_ID, _("ID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_LabelID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_ID = new wxTextCtrl( m_GeneralPanel, ID_TEXTCTRL_TEXT_ID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_ID, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelText = new wxStaticText( m_GeneralPanel, ID_STATICTEXT_TEXT_TEXT, _("Text"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_LabelText, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM|wxADJUST_MINSIZE, 5);

    m_Text = new wxTextCtrl( m_GeneralPanel, ID_TEXTCTRL_TEXT_TEXT, wxEmptyString, wxDefaultPosition, wxSize(-1, 80), wxTE_MULTILINE );
    itemFlexGridSizer12->Add(m_Text, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labImage = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Image"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_labImage, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_ImageButton = new wxButton( m_GeneralPanel, ID_BUTTON_TXT_IMAGE, _("..."), wxDefaultPosition, wxSize(50, 25), 0 );
    itemFlexGridSizer12->Add(m_ImageButton, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_LabelTip = new wxStaticText( m_GeneralPanel, ID_STATICTEXT_TEXT_TIP, _("Tip"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_LabelTip, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM|wxADJUST_MINSIZE, 5);

    m_Tip = new wxTextCtrl( m_GeneralPanel, ID_TEXTCTRL_TEXT_TIP, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_Tip, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labPointsize = new wxStaticText( m_GeneralPanel, wxID_STATIC_TX_POINT, _("pointsize"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_labPointsize, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM|wxADJUST_MINSIZE, 5);

    m_Pointsize = new wxTextCtrl( m_GeneralPanel, ID_TEXTCTRL_TX_POINT, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer12->Add(m_Pointsize, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    itemFlexGridSizer12->AddGrowableCol(1);

    wxFlexGridSizer* itemFlexGridSizer23 = new wxFlexGridSizer(0, 4, 0, 0);
    itemBoxSizer11->Add(itemFlexGridSizer23, 0, wxGROW|wxALL, 5);
    m_labColor = new wxStaticText( m_GeneralPanel, wxID_ANY, _("color"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_labColor, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Color = new wxButton( m_GeneralPanel, ID_BUTTON_TEXT_COLOR, _("..."), wxDefaultPosition, wxSize(50, 25), 0 );
    itemFlexGridSizer23->Add(m_Color, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labBackColor = new wxStaticText( m_GeneralPanel, wxID_ANY, _("background"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_labBackColor, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Background = new wxButton( m_GeneralPanel, ID_BUTTON_TEXT_BACKGROUND, _("..."), wxDefaultPosition, wxSize(50, 25), 0 );
    itemFlexGridSizer23->Add(m_Background, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_AttributesBox = new wxStaticBox(m_GeneralPanel, wxID_ANY, _("Attributes"));
    wxStaticBoxSizer* itemStaticBoxSizer28 = new wxStaticBoxSizer(m_AttributesBox, wxHORIZONTAL);
    itemBoxSizer11->Add(itemStaticBoxSizer28, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer29 = new wxFlexGridSizer(0, 4, 0, 0);
    itemStaticBoxSizer28->Add(itemFlexGridSizer29, 1, wxALIGN_CENTER_VERTICAL, 5);
    m_Bold = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("bold"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Bold->SetValue(false);
    itemFlexGridSizer29->Add(m_Bold, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Underlined = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("underlined"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Underlined->SetValue(false);
    itemFlexGridSizer29->Add(m_Underlined, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Italic = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("italic"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Italic->SetValue(false);
    itemFlexGridSizer29->Add(m_Italic, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Transparent = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("transparent"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Transparent->SetValue(false);
    itemFlexGridSizer29->Add(m_Transparent, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Input = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("Input"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Input->SetValue(false);
    itemFlexGridSizer29->Add(m_Input, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_Border = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("border"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Border->SetValue(false);
    itemFlexGridSizer29->Add(m_Border, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_Clock = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("Clock"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Clock->SetValue(false);
    itemFlexGridSizer29->Add(m_Clock, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    wxFlexGridSizer* itemFlexGridSizer37 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer11->Add(itemFlexGridSizer37, 0, wxALIGN_LEFT, 5);
    wxArrayString m_OriStrings;
    m_OriStrings.Add(_("&default"));
    m_OriStrings.Add(_("&up"));
    m_OriStrings.Add(_("&down"));
    m_Ori = new wxRadioBox( m_GeneralPanel, wxID_ANY, _("Orientation"), wxDefaultPosition, wxDefaultSize, m_OriStrings, 1, wxRA_SPECIFY_ROWS );
    m_Ori->SetSelection(0);
    itemFlexGridSizer37->Add(m_Ori, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_Reset = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("Reset"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Reset->SetValue(false);
    itemFlexGridSizer37->Add(m_Reset, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_Actions = new wxButton( m_GeneralPanel, ID_TEXT_ACTIONS, _("Actions..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(m_Actions, 0, wxALIGN_LEFT|wxALL, 5);

    m_Notebook->AddPage(m_GeneralPanel, _("General"));

    m_InterfacePanel = new wxPanel( m_Notebook, ID_INT_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer42 = new wxBoxSizer(wxVERTICAL);
    m_InterfacePanel->SetSizer(itemBoxSizer42);

    wxFlexGridSizer* itemFlexGridSizer43 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer42->Add(itemFlexGridSizer43, 0, wxGROW|wxALL, 5);
    m_labIID = new wxStaticText( m_InterfacePanel, wxID_ANY, _("IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer43->Add(m_labIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_IIDStrings;
    m_IID = new wxComboBox( m_InterfacePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_IIDStrings, wxCB_DROPDOWN );
    itemFlexGridSizer43->Add(m_IID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labBus = new wxStaticText( m_InterfacePanel, wxID_ANY, _("Bus"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer43->Add(m_labBus, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Bus = new wxTextCtrl( m_InterfacePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(160, -1), 0 );
    itemFlexGridSizer43->Add(m_Bus, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labAddress = new wxStaticText( m_InterfacePanel, wxID_ANY, _("Address"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer43->Add(m_labAddress, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Address = new wxSpinCtrl( m_InterfacePanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize(120, -1), wxSP_ARROW_KEYS, 0, 65535, 0 );
    itemFlexGridSizer43->Add(m_Address, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labDisplay = new wxStaticText( m_InterfacePanel, wxID_ANY, _("Display"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer43->Add(m_labDisplay, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Display = new wxSpinCtrl( m_InterfacePanel, wxID_ANY, wxT("1"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 1, 8, 1 );
    itemFlexGridSizer43->Add(m_Display, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemFlexGridSizer43->AddGrowableCol(1);

    m_Notebook->AddPage(m_InterfacePanel, _("Interface"));

    m_LocationPanel = new wxPanel( m_Notebook, ID_LOC_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxFlexGridSizer* itemFlexGridSizer53 = new wxFlexGridSizer(0, 2, 0, 0);
    m_LocationPanel->SetSizer(itemFlexGridSizer53);

    m_LabelX = new wxStaticText( m_LocationPanel, ID_STATICTEXT_TEXT_X, _("x"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer53->Add(m_LabelX, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_x = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL_TEXT_X, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer53->Add(m_x, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelY = new wxStaticText( m_LocationPanel, ID_STATICTEXT_TEXT_Y, _("y"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer53->Add(m_LabelY, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_y = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL_TEXT_Y, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer53->Add(m_y, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelZ = new wxStaticText( m_LocationPanel, ID_STATICTEXT_TEXT_Z, _("z"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer53->Add(m_LabelZ, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_z = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL_TEXT_Z, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer53->Add(m_z, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelCx = new wxStaticText( m_LocationPanel, wxID_STATICTEXT_TEXT_CX, _("cx"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer53->Add(m_LabelCx, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Cx = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL_TEXT_CX, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer53->Add(m_Cx, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelCy = new wxStaticText( m_LocationPanel, wxID_STATICTEXT_TEXT_CY, _("cy"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer53->Add(m_LabelCy, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Cy = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL_TEXT_CY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer53->Add(m_Cy, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_LocationPanel, _("Location"));

    itemBoxSizer2->Add(m_Notebook, 0, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer64 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer64, 0, wxGROW|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer64->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer64->AddButton(m_Cancel);

    m_Help = new wxButton( itemDialog1, wxID_HELP, _("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer64->AddButton(m_Help);

    m_Apply = new wxButton( itemDialog1, wxID_APPLY, _("&Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer64->AddButton(m_Apply);

    itemStdDialogButtonSizer64->Realize();

////@end TextDialog content construction
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void TextDialog::OnOkClick( wxCommandEvent& event )
{
  if( !m_Props )
    return;
  if( !evaluate() )
    return;
  if( !wxGetApp().isStayOffline() ) {
    /* Notify RocRail. */
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.modify );
    NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  else {
    wxGetApp().setLocalModelModified(true);
  }
  EndModal( wxID_OK );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void TextDialog::OnCancelClick( wxCommandEvent& event ){
  EndModal( 0 );
}

/*!
 * Should we show tooltips?
 */

bool TextDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap TextDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin TextDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end TextDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon TextDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin TextDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end TextDialog icon retrieval
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_TXT_IMAGE
 */

void TextDialog::OnButtonTxtImageClick( wxCommandEvent& event )
{
  const char* imagepath = wGui.getimagepath( wxGetApp().getIni() );
  TraceOp.trc( "textdialog", TRCLEVEL_INFO, __LINE__, 9999, "imagepath = [%s]", imagepath );
  wxFileDialog* fdlg = new wxFileDialog(this, _T("Search image"),
      wxString(imagepath,wxConvUTF8), _T(""),
      _T("PNG files (*.png)|*.png"), wxFD_OPEN);
  if( fdlg->ShowModal() == wxID_OK ) {
    TraceOp.trc( "textdialog", TRCLEVEL_INFO, __LINE__, 9999, "Loading %s...", (const char*)fdlg->GetPath().mb_str(wxConvUTF8) );
    wxBitmapType bmptype = wxBITMAP_TYPE_XPM;
    if( StrOp.endsWithi( fdlg->GetPath().mb_str(wxConvUTF8), ".gif" ) )
      bmptype = wxBITMAP_TYPE_GIF;
    else if( StrOp.endsWithi( fdlg->GetPath().mb_str(wxConvUTF8), ".png" ) )
      bmptype = wxBITMAP_TYPE_PNG;
    wText.settext( m_Props, FileOp.ripPath(fdlg->GetPath().mb_str(wxConvUTF8)) );
    m_Text->SetValue( wxString(wText.gettext( m_Props ),wxConvUTF8) );
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_TEXT_COLOR
 */

void TextDialog::OnButtonTextColorClick( wxCommandEvent& event )
{
  wxColourDialog* dlg = new wxColourDialog(this);
  if( wxID_OK == dlg->ShowModal() ) {
    wxColour &color = dlg->GetColourData().GetColour();

    wText.setred( m_Props, (int)color.Red() );
    wText.setgreen( m_Props, (int)color.Green() );
    wText.setblue( m_Props, (int)color.Blue() );

    m_Color->SetBackgroundColour( color );

  }
  dlg->Destroy();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_TEXT_BACKGROUND
 */

void TextDialog::OnButtonTextBackgroundClick( wxCommandEvent& event )
{
  wxColourDialog* dlg = new wxColourDialog(this);
  if( wxID_OK == dlg->ShowModal() ) {
    wxColour &color = dlg->GetColourData().GetColour();

    wText.setbackred( m_Props, (int)color.Red() );
    wText.setbackgreen( m_Props, (int)color.Green() );
    wText.setbackblue( m_Props, (int)color.Blue() );
    m_Background->SetBackgroundColour( color );

  }
  dlg->Destroy();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_TEXT_ACTIONS
 */

void TextDialog::OnTextActionsClick( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  ActionsCtrlDlg*  dlg = new ActionsCtrlDlg(this, m_Props );

  if( wxID_OK == dlg->ShowModal() ) {
    // TODO: inform
  }

  dlg->Destroy();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
 */

void TextDialog::OnHelpClick( wxCommandEvent& event )
{
  wxGetApp().openLink( "text" );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_NEWTEXT
 */

void TextDialog::OnNewtextClick( wxCommandEvent& event )
{
  int i = findID("NEW");
  if( i == wxNOT_FOUND ) {
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode txlist = wPlan.gettxlist( model );
      if( txlist == NULL ) {
        txlist = NodeOp.inst( wTextList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, txlist );
      }
      if( txlist != NULL ) {
        iONode tx = NodeOp.inst( wText.name(), txlist, ELEMENT_NODE );
        NodeOp.addChild( txlist, tx );
        wText.setid( tx, "NEW" );
        appendItem(tx);
        setIDSelection(wItem.getid(tx));
        m_Props = tx;
        initValues();
      }
    }
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_DELETETEXT
 */

void TextDialog::OnDeletetextClick( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  int action = wxMessageDialog( this, wxGetApp().getMsg("removewarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
  if( action == wxID_NO )
    return;

  wxGetApp().pushUndoItem( (iONode)NodeOp.base.clone( m_Props ) );

  /* Notify RocRail. */
  iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
  wModelCmd.setcmd( cmd, wModelCmd.remove );
  NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode txlist = wPlan.gettxlist( model );
    if( txlist != NULL ) {
      NodeOp.removeChild( txlist, m_Props );
      m_Props = NULL;
    }
  }

  initIndex();
}


/*!
 * wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_INDEXLIST
 */

void TextDialog::OnIndexlistSelected( wxListEvent& event )
{
  m_Props = getSelection(event.GetIndex());
  initValues();
  m_Delete->Enable( true );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
 */

void TextDialog::OnApplyClick( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;
  if( !evaluate() )
    return;

  if( !wxGetApp().isStayOffline() ) {
    /* Notify RocRail. */
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.modify );
    NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  else {
    wxGetApp().setLocalModelModified(true);
  }
  initIndex();
}

