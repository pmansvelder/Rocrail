/*
 Rocrail - Model Railroad Software

 Copyright (c) 2002 Robert Jan Versluis, Rocrail.net

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#include "dmxdlg.h"

#include "rocview/public/guiapp.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/DMX.h"

DmxDlg::DmxDlg( wxWindow* parent, iONode props ):dmxdlggen( parent )
{
  m_Props = props;
  initLabels();
  // Resize
  GetSizer()->Layout();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
  initValues();
}

void DmxDlg::onCancel( wxCommandEvent& event )
{
  EndModal(0);
}

void DmxDlg::onHelp( wxCommandEvent& event )
{
  wxGetApp().openLink( "dmx:artnet" );
}

void DmxDlg::onOK( wxCommandEvent& event )
{
  evaluate();
  EndModal( wxID_OK );
}

void DmxDlg::initLabels() {
  m_labIID->SetLabel(wxGetApp().getMsg( "iid" ));
  m_labHost->SetLabel(wxGetApp().getMsg( "host" ));
  m_labFrameRate->SetLabel(wxGetApp().getMsg( "framerate" ));
  m_labFrameRateMS->SetLabel(wxGetApp().getMsg( "milliseconds" ));
  m_MapWhite->SetLabel(wxGetApp().getMsg( "mapwhite" ));
  m_RestoreData->SetLabel(wxGetApp().getMsg( "restorechannels" ));

  // Buttons
  m_stdButtonsOK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_stdButtonsCancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  m_stdButtonsHelp->SetLabel( wxGetApp().getMsg( "help" ) );
}

void DmxDlg::initValues() {
  iONode dmxini = wDigInt.getdmx(m_Props);
  if( dmxini == NULL ) {
    dmxini = NodeOp.inst(wDMX.name(), m_Props, ELEMENT_NODE);
    NodeOp.addChild(m_Props, dmxini);
  }

  m_IID->SetValue( wxString( wDigInt.getiid( m_Props ), wxConvUTF8 ) );
  m_Host->SetValue( wxString( wDigInt.gethost( m_Props ), wxConvUTF8 ) );
  m_FrameRate->SetValue(wDMX.getframerate(dmxini));
  m_MapWhite->SetValue(wDMX.ismapwhite(dmxini)?true:false);
  m_RestoreData->SetValue(wDMX.isrestoredata(dmxini)?true:false);
}

bool DmxDlg::evaluate() {
  iONode dmxini = wDigInt.getdmx(m_Props);
  wDigInt.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );
  wDigInt.sethost( m_Props, m_Host->GetValue().mb_str(wxConvUTF8) );
  wDMX.setframerate(dmxini, m_FrameRate->GetValue());
  wDMX.setmapwhite(dmxini, m_MapWhite->IsChecked()?True:False);
  wDMX.setrestoredata(dmxini, m_RestoreData->IsChecked()?True:False);
  return true;
}

