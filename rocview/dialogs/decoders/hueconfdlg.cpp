/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2015 Rob Versluis, Rocrail.net

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "hueconfdlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/guiapp.h"
#include "rocview/dialogs/basedlg.h"

#include "rocrail/wrapper/public/Program.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/str.h"


HueConfDlg::HueConfDlg( wxWindow* parent ):HueConfDlgGen( parent )
{
  initLabels();

  m_LightsPanel->GetSizer()->Layout();

  m_HueBook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  if( m_LightsGrid->GetNumberRows() > 0 )
    m_LightsGrid->DeleteRows( 0, m_LightsGrid->GetNumberRows() );


}


void HueConfDlg::onFindLights( wxCommandEvent& event ) {
  TraceOp.trc( "hueconf", TRCLEVEL_INFO, __LINE__, 9999, "find lights..." );
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.update );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_hue);
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void HueConfDlg::onGetLights( wxCommandEvent& event ) {
  TraceOp.trc( "hueconf", TRCLEVEL_INFO, __LINE__, 9999, "get lights..." );
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.query );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_hue);
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void HueConfDlg::onHelp( wxCommandEvent& event ) {
  switch( m_HueBook->GetSelection() ) {
  case 0: wxGetApp().openLink( "philips:hue" ); break;
  default: wxGetApp().openLink( "philips:hue" ); break;
  }
}

void HueConfDlg::onOK( wxCommandEvent& event ) {
  //EndModal( wxID_OK );
  wxGetApp().getFrame()->resetHueConfRef();
  Destroy();
}

void HueConfDlg::onClose( wxCloseEvent& event ) {
  //EndModal( wxID_OK );
  wxGetApp().getFrame()->resetHueConfRef();
  Destroy();
}

void HueConfDlg::initLabels() {
  SetTitle(wxT("HUE"));
  m_HueBook->SetPageText( 0, wxGetApp().getMsg( "lights" ) );

  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  BaseDialog::addIIDs(m_IID);

  // Lights
  m_LightsGrid->SetColLabelValue(0, wxGetApp().getMsg("address") );
  m_LightsGrid->SetColLabelValue(1, wxGetApp().getMsg("name") );
  m_LightsGrid->SetColLabelValue(2, wxGetApp().getMsg("type") );
  m_LightsGrid->SetColLabelValue(3, wxGetApp().getMsg("available") );

  m_GetLights->SetLabel( wxGetApp().getMsg( "get" ) );
  m_FindLights->SetLabel( wxGetApp().getMsg( "query" ) );

  // Buttons
  m_stdButtonsOK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_stdButtonsHelp->SetLabel( wxGetApp().getMsg( "help" ) );
}

void HueConfDlg::event(iONode node) {
  if( m_LightsGrid->GetNumberRows() > 0 )
    m_LightsGrid->DeleteRows( 0, m_LightsGrid->GetNumberRows() );
  /*
  <program cmd="7" lntype="9" iid="hue-1">
    <json>
      <1 type="Dimmable light" name="Lux Lamp" modelid="LWB004" uniqueid="0017880100de2419-0b" swversion="66012040">
        <state on="false" bri="0" alert="none" reachable="false"/>
        <pointsymbol 1="none" 2="none" 3="none" 4="none" 5="none" 6="none" 7="none" 8="none"/>
      </1>
   */
  char* s = NodeOp.base.toString(node);
  TraceOp.trc( "hueconf", TRCLEVEL_INFO, __LINE__, 9999, "event:\n%s", s );
  StrOp.free(s);

  iONode json = NodeOp.findNode(node, "json");
  if( json != NULL ) {
    int cnt = NodeOp.getChildCnt(json);
    for( int i = 0; i < cnt; i++ ) {
      iONode light = NodeOp.getChild(json, i);
      m_LightsGrid->AppendRows();
      int row = m_LightsGrid->GetNumberRows()-1;
      m_LightsGrid->SetCellValue(row, 0, wxString(NodeOp.getName(light),wxConvUTF8) );
      m_LightsGrid->SetCellAlignment(wxALIGN_CENTRE, row, 0);

      m_LightsGrid->SetCellValue(row, 1, wxString(NodeOp.getStr(light, "name", "?"),wxConvUTF8) );
      m_LightsGrid->SetCellValue(row, 2, wxString(NodeOp.getStr(light, "type", "?"),wxConvUTF8) );
      iONode state = NodeOp.findNode(light, "state");
      if( state != NULL ) {
        m_LightsGrid->SetCellValue(row, 3, wxString(NodeOp.getStr(state, "reachable", "?"),wxConvUTF8) );
        m_LightsGrid->SetCellAlignment(wxALIGN_CENTRE, row, 3);
      }
    }
    m_LightsGrid->AutoSize();
    m_LightsPanel->GetSizer()->Layout();
  }


}

void HueConfDlg::onLightCellChange( wxGridEvent& event ) {

}

