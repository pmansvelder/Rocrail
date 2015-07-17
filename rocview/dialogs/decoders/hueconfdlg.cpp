/*
 Rocrail - Model Railroad Software

 Copyright (c) 2002 Robert Jan Versluis, Rocrail.net

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

#include <wx/colordlg.h>

#include "rocview/public/guiapp.h"
#include "rocview/dialogs/basedlg.h"

#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Color.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/str.h"

#include "rocview/res/icons.hpp"


HueConfDlg::HueConfDlg( wxWindow* parent ):HueConfDlgGen( parent )
{
  m_Row = -1;
  m_Col = -1;

  initLabels();

  m_BridgePanel->GetSizer()->Layout();
  m_LightsPanel->GetSizer()->Layout();

  m_HueBook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  if( m_LightsGrid->GetNumberRows() > 0 )
    m_LightsGrid->DeleteRows( 0, m_LightsGrid->GetNumberRows() );

  m_SetLight->Enable(false);
  m_LightOn->Enable(false);
  m_LightOff->Enable(false);
  m_Brightness->Enable(false);
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
  case 0: wxGetApp().openLink( "philips:hue-conf", "bridge" ); break;
  case 1: wxGetApp().openLink( "philips:hue-conf", "lights" ); break;
  default: wxGetApp().openLink( "philips:hue-conf" ); break;
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
  SetTitle(wxT("Philips HUE"));
  m_HueBook->SetPageText( 0, wxGetApp().getMsg( "bridge" ) );
  m_HueBook->SetPageText( 1, wxGetApp().getMsg( "lights" ) );

  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  BaseDialog::addIIDs(m_IID);

  m_HueLogo->SetBitmap(*_img_hue_logo);

  // Bridge
  m_labBridgeName->SetLabel( wxGetApp().getMsg( "name" ) );
  m_SetBridgeName->SetLabel( wxGetApp().getMsg( "set" ) );
  m_GetBridge->SetLabel( wxGetApp().getMsg( "get" ) );
  m_labBridgeUserName->SetLabel( wxGetApp().getMsg( "userid" ) );
  m_SetBridgeUserName->SetLabel( wxGetApp().getMsg( "set" ) );


  // Lights
  m_LightsGrid->SetColLabelValue(0, wxGetApp().getMsg("address") );
  m_LightsGrid->SetColLabelValue(1, wxGetApp().getMsg("name") );
  m_LightsGrid->SetColLabelValue(2, wxGetApp().getMsg("type") );
  m_LightsGrid->SetColLabelValue(3, wxGetApp().getMsg("available") );
  m_LightsGrid->SetColLabelValue(4, wxGetApp().getMsg("brightness") );

  m_GetLights->SetLabel( wxGetApp().getMsg( "get" ) );
  m_FindLights->SetLabel( wxGetApp().getMsg( "query" ) );
  m_SetLight->SetLabel( wxGetApp().getMsg( "set" ) );
  m_LightOn->SetLabel( wxGetApp().getMsg( "on" ) );
  m_LightOff->SetLabel( wxGetApp().getMsg( "off" ) );

  // Buttons
  m_stdButtonsOK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_stdButtonsHelp->SetLabel( wxGetApp().getMsg( "help" ) );
}

void HueConfDlg::event(iONode node) {
  m_Row = -1;
  m_Col = -1;

  char* s = NodeOp.base.toString(node);
  TraceOp.trc( "hueconf", TRCLEVEL_INFO, __LINE__, 9999, "event:\n%s", s );
  StrOp.free(s);

  int cv = wProgram.getcv(node);

  iONode json = NodeOp.findNode(node, "json");

  // Bridge config
  if( json != NULL && cv == 0) {
    /*
    <program cmd="7" cv="0" lntype="9" iid="hue-1">
      <json name="Philips hue" zigbeechannel="20" mac="0017881215cd" dhcp="false" ipaddress="192.168.100.175" netmask="255.255.255.0"
      gateway="192.168.100.99" proxyaddress="none" proxyport="0" UTC="2015-02-05T132537" localtime="2015-02-05T142537"
      timezone="Europe/Berlin" swversion="01018228" apiversion="1.5.0" linkbutton="false" portalservices="true" portalconnection="connected">
        <whitelist>
          <000000007b9c25875ddd6a1a5ddd6a1a last="" use="" date="2015-02-03T194638" create="" date="2014-12-04T160318" name="Hue#BullittGroupLimited"/>
          <rocrail4ever last="" use="" date="2014-12-04T161156" create="" date="2014-12-04T161156" name="rocrail"/>
          <rocrail4light last="" use="" date="2015-02-05T132537" create="" date="2014-12-04T161346" name="rocrail"/>
          <Druxcrfl7zVWbckI last="" use="" date="2014-12-11T153548" create="" date="2014-12-07T150148" name="Huemanic"/>
        </whitelist>
        <swupdate updatestate="0" checkforupdate="false" url="" text="" notify="false">
          <devicetypes bridge="false" lights=""/>
        </swupdate>
        <portalstate signedon="true" incoming="true" outgoing="true" communication="disconnected"/>
      </json>
    </program>
     */
    m_BridgeName->SetValue(wxString(NodeOp.getStr(json, "name", "?"),wxConvUTF8) );
    m_BridgeIP->SetValue(wxString(NodeOp.getStr(json, "ipaddress", "?"),wxConvUTF8) );
    m_BridgeFirmware->SetValue(wxString(NodeOp.getStr(json, "swversion", "?"),wxConvUTF8) );
    m_BridgeAPI->SetValue(wxString(NodeOp.getStr(json, "apiversion", "?"),wxConvUTF8) );
    m_BridgeZigbee->SetValue(wxString(NodeOp.getStr(json, "zigbeechannel", "?"),wxConvUTF8) );
    m_BridgeUserName->Clear();
    iONode users = NodeOp.findNode(json, "whitelist");
    if( users != NULL ) {
      int cnt = NodeOp.getChildCnt(users);
      for( int i = 0; i < cnt; i++ ) {
        iONode user = NodeOp.getChild( users, i );
        m_BridgeUserName->Append(wxString(NodeOp.getName(user),wxConvUTF8));
      }
    }
  }

  // Lights
  else if( json != NULL && cv == 1024 ) {
    if( m_LightsGrid->GetNumberRows() > 0 )
      m_LightsGrid->DeleteRows( 0, m_LightsGrid->GetNumberRows() );
    /*
    <program cmd="7" cv="1024" lntype="9" iid="hue-1">
      <json>
        <1 type="Dimmable light" name="Lux Lamp" modelid="LWB004" uniqueid="0017880100de2419-0b" swversion="66012040">
          <state on="false" bri="0" alert="none" reachable="false"/>
          <state on="true" bri="254" hue="0" sat="254" effect="none" xy="0.70400.2960" alert="none" colormode="xy" reachable="true"/>
          <pointsymbol 1="none" 2="none" 3="none" 4="none" 5="none" 6="none" 7="none" 8="none"/>
        </1>

        "xy":[0.4291,0.4144]
     */
    int cnt = NodeOp.getChildCnt(json);
    for( int i = 0; i < cnt; i++ ) {
      iONode light = NodeOp.getChild(json, i);
      m_LightsGrid->AppendRows();
      int row = m_LightsGrid->GetNumberRows()-1;
      m_LightsGrid->SetCellValue(row, 0, wxString(NodeOp.getName(light),wxConvUTF8) );
      m_LightsGrid->SetCellAlignment(wxALIGN_CENTRE, row, 0);
      m_LightsGrid->SetReadOnly( row, 0, true );

      m_LightsGrid->SetCellValue(row, 1, wxString(NodeOp.getStr(light, "name", "?"),wxConvUTF8) );
      m_LightsGrid->SetCellValue(row, 2, wxString(NodeOp.getStr(light, "type", "?"),wxConvUTF8) );
      m_LightsGrid->SetReadOnly( row, 2, true );
      iONode state = NodeOp.findNode(light, "state");
      if( state != NULL ) {
        m_LightsGrid->SetCellValue(row, 3, wxString(NodeOp.getStr(state, "reachable", "?"),wxConvUTF8) );
        m_LightsGrid->SetCellAlignment(wxALIGN_CENTRE, row, 3);
        m_LightsGrid->SetReadOnly( row, 3, true );
        m_LightsGrid->SetCellValue(row, 4, wxString(NodeOp.getStr(state, "bri", "0"),wxConvUTF8) );
        m_LightsGrid->SetCellAlignment(wxALIGN_CENTRE, row, 4);
        m_LightsGrid->SetReadOnly( row, 4, true );
      }
    }
    m_LightsGrid->AutoSize();
    m_LightsPanel->GetSizer()->Layout();
  }


}

void HueConfDlg::onLightSet( wxCommandEvent& event ) {
  if( m_Row == -1 || m_Col == -1 )
    return;

  TraceOp.trc( "hueconf", TRCLEVEL_INFO, __LINE__, 9999, "program cell: %d,%d", m_Row, m_Col );
  if( m_Col == 1 ) {
    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setcmd( cmd, wProgram.setstring );
    wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wProgram.setlntype(cmd, wProgram.lntype_hue);
    wProgram.setcv(cmd, atoi(m_LightsGrid->GetCellValue(m_Row, 0).mb_str(wxConvUTF8)));
    wProgram.setstrval1(cmd, m_LightsGrid->GetCellValue(m_Row, m_Col).mb_str(wxConvUTF8));
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);

    wxCommandEvent cevent;
    m_labChangedLight->SetLabel(wxT("..."));
    m_SetLight->Enable(false);

    onGetLights(cevent);
  }
}

void HueConfDlg::onLightCellChange( wxGridEvent& event ) {
  m_Row = event.GetRow();
  m_Col = event.GetCol();
  TraceOp.trc( "hueconf", TRCLEVEL_INFO, __LINE__, 9999, "cell changed: %d,%d", m_Row, m_Col );
  m_labChangedLight->SetLabel( m_LightsGrid->GetCellValue(m_Row, 0) + wxT(": ") + m_LightsGrid->GetCellValue(m_Row, 1) );
  m_SetLight->Enable(true);
}


void HueConfDlg::onLightCellDClick( wxGridEvent& event ) {
  m_Row = event.GetRow();
  m_Col = event.GetCol();
  m_LightOn->Enable(true);
  m_LightOff->Enable(true);
  m_Brightness->Enable(true);

  int bri = atoi(m_LightsGrid->GetCellValue(m_Row, 4).mb_str(wxConvUTF8));
  m_Brightness->SetValue(bri);

  if( StrOp.find(m_LightsGrid->GetCellValue(m_Row, 2).mb_str(wxConvUTF8), "olor" ) ) {
    wxColourData ColourData;
    ColourData.SetColour(wxColour(255,255,255));

    wxColourDialog* dlg = new wxColourDialog(this, &ColourData);
    if( wxID_OK == dlg->ShowModal() ) {
      wxColour &colour = dlg->GetColourData().GetColour();

      iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE);
      iONode color = NodeOp.inst( wColor.name(), NULL, ELEMENT_NODE);
      NodeOp.addChild(cmd, color);
      wOutput.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
      wOutput.setporttype( cmd, wProgram.porttype_light );
      wOutput.setcolortype( cmd, True );
      wColor.setred(color, (int)colour.Red());
      wColor.setgreen(color, (int)colour.Green());
      wColor.setblue(color, (int)colour.Blue());
      wOutput.setaddr( cmd, atoi(m_LightsGrid->GetCellValue(m_Row, 0).mb_str(wxConvUTF8)) );
      wOutput.setvalue(cmd, 255);
      wOutput.setcmd(cmd, wOutput.value);
      wOutput.setstate( cmd, wOutput.on );
      wxGetApp().sendToRocrail( cmd );
      cmd->base.del(cmd);
    }
    dlg->Destroy();
  }
}



void HueConfDlg::onBridgeNameSet( wxCommandEvent& event ) {
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.setstring );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_hue);
  wProgram.setcv(cmd, 0);
  wProgram.setstrval1(cmd, m_BridgeName->GetValue().mb_str(wxConvUTF8));
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void HueConfDlg::onBridgeGet( wxCommandEvent& event ) {
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.getoptions );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_hue);
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void HueConfDlg::onSetBridgeUserName( wxCommandEvent& event ) {
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.setstring );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_hue);
  wProgram.setcv(cmd, 1025);
  wProgram.setstrval1(cmd, m_BridgeUserName->GetValue().mb_str(wxConvUTF8));
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void HueConfDlg::onLightOn( wxCommandEvent& event ) {
  if( m_Row != -1 ) {
    iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE);
    wOutput.setaddr( cmd, atoi(m_LightsGrid->GetCellValue(m_Row, 0).mb_str(wxConvUTF8)) );
    wOutput.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wOutput.setcmd( cmd, wOutput.on );
    wOutput.setvalue( cmd, atoi(m_LightsGrid->GetCellValue(m_Row, 4).mb_str(wxConvUTF8)) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}


void HueConfDlg::onLightOff( wxCommandEvent& event ) {
  if( m_Row != -1 ) {
    iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE);
    wOutput.setaddr( cmd, atoi(m_LightsGrid->GetCellValue(m_Row, 0).mb_str(wxConvUTF8)) );
    wOutput.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wOutput.setcmd( cmd, wOutput.off );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}


void HueConfDlg::onLightCellSelect( wxGridEvent& event ) {
  m_Row = event.GetRow();
  m_Col = event.GetCol();
  m_LightOn->Enable(true);
  m_LightOff->Enable(true);
  m_Brightness->Enable(true);
  int bri = atoi(m_LightsGrid->GetCellValue(m_Row, 4).mb_str(wxConvUTF8));
  m_Brightness->SetValue(bri);
}


void HueConfDlg::onBrightness( wxScrollEvent& event ) {
  if( m_Row != -1 ) {
    m_LightsGrid->SetCellValue(m_Row, 4, wxString::Format(wxT("%d"), m_Brightness->GetValue()) );
  }
}

void HueConfDlg::onBrightnessThumbRelease( wxScrollEvent& event ) {
  if( m_Row != -1 ) {
    wxCommandEvent cmdevent;
    onLightOn(cmdevent);
  }
}

