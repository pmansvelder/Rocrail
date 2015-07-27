/*
 Rocrail - Model Railroad Software
 Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net
 All rights reserved.
*/

#include "ucons88dlg.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif


#include "rocview/public/guiapp.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/uConS88.h"
//#include "rocrail/wrapper/public/uConS88node.h"

#include "rocutils/public/vendors.h"

#include "rocs/public/strtok.h"

uConS88Dlg::uConS88Dlg( wxWindow* parent, iONode props, const char* devices )
:
ucons88dlggen( parent ) {
  m_Props = props;
  m_Devices = devices;
  __initVendors();
  initLabels();
  initValues();

//  m_General->GetSizer()->Layout();
//  m_Options->GetSizer()->Layout();

//  m_General->Fit();
//  m_Options->Fit();

  GetSizer()->Layout();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
}

void uConS88Dlg::initLabels() {
  // General
  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );

  m_DeviceBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "device" ) );
  m_labHost->SetLabel( wxGetApp().getMsg( "host" ) );
  m_labPort->SetLabel( wxGetApp().getMsg( "port" ) );

  m_S88BusBox->GetStaticBox()->SetLabel( _T("S88 ") + wxGetApp().getMsg( "bus" ) );

  m_labBus1->SetLabel( wxString::Format(wxGetApp().getMsg( "buslength" ), 1) );
  m_labBus2->SetLabel( wxString::Format(wxGetApp().getMsg( "buslength" ), 2) );
  m_labBus3->SetLabel( wxString::Format(wxGetApp().getMsg( "buslength" ), 3) );

  // Buttons
  m_StdButtonOK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_StdButtonCancel->SetLabel( wxGetApp().getMsg( "cancel" ) );

}

void uConS88Dlg::initValues() {
  if( m_Props == NULL )
    return;

  iONode ucons88ini = wDigInt.getucons88(m_Props);
  if( ucons88ini == NULL ) {
    ucons88ini = NodeOp.inst( wuConS88.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild(m_Props, ucons88ini );
  }

  // General
  m_IID->SetValue( wxString( wDigInt.getiid( m_Props ), wxConvUTF8 ) );

  if ( wuConS88.getport( m_Props ) > 0 ) {
    m_Host->SetValue( wxString( wuConS88.gethost( m_Props ), wxConvUTF8 ) );
    m_Port->SetValue( wuConS88.getport( m_Props ) );
    m_B1->SetValue( wuConS88.getfbB1( m_Props ) );
    m_B2->SetValue( wuConS88.getfbB2( m_Props ) );
    m_B3->SetValue( wuConS88.getfbB2( m_Props ) );
  } else {
    m_Host->SetValue( wxString( wuConS88.gethost( ucons88ini ), wxConvUTF8 ) );
    m_Port->SetValue( wuConS88.getport( ucons88ini ) );
    m_B1->SetValue( wuConS88.getfbB1( ucons88ini ) );
    m_B2->SetValue( wuConS88.getfbB2( ucons88ini ) );
    m_B3->SetValue( wuConS88.getfbB2( ucons88ini ) );
  }
  
}

void uConS88Dlg::evaluate() {
  if( m_Props == NULL )
    return;

  // General
  wDigInt.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );
  wDigInt.sethost( m_Props, m_Host->GetValue().mb_str(wxConvUTF8) );
  wDigInt.setport( m_Props, m_Port->GetValue() );

  wuConS88.setfbB1( m_Props, m_B1->GetValue() );
  wuConS88.setfbB2( m_Props, m_B2->GetValue() );
  wuConS88.setfbB3( m_Props, m_B3->GetValue() );

}

void uConS88Dlg::OnCancel( wxCommandEvent& event )
{
  EndModal( wxID_CANCEL );
}

void uConS88Dlg::OnOK( wxCommandEvent& event )
{
  evaluate();
  EndModal( wxID_OK );
}

void uConS88Dlg::OnHelp( wxCommandEvent& event )
{
  wxGetApp().openLink( "ucons88" );
}
