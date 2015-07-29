/*
 Rocrail - Model Railroad Software
 Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net
 All rights reserved.
*/

#include "xmlscriptdlg.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/guiapp.h"
#include "rocview/public/base.h"
#include "rocrail/wrapper/public/DataReq.h"
#include "rocs/public/doc.h"
#include "rocs/public/node.h"

XmlScriptDlg::XmlScriptDlg( wxWindow* parent, iONode node ):xmlscriptdlggen( parent )
{
  m_Data = node;
  initLabels();
  GetSizer()->Layout();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  char* title = StrOp.fmt("XmlScript: %s", wDataReq.getfilename(m_Data) );
  SetTitle(wxString(title,wxConvUTF8) );
  StrOp.free(title);

  m_XML->SetValue( wxString(wDataReq.getdata(node),wxConvUTF8) );
}

void XmlScriptDlg::initLabels() {
  m_Validate->SetLabel( wxGetApp().getMsg( "validate" ) );
  // Buttons
  m_stdButtonSave->SetLabel( wxGetApp().getMsg( "save" ) );
  m_stdButtonCancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
}

void XmlScriptDlg::onCancel( wxCommandEvent& event )
{
  EndModal(0);
}
void XmlScriptDlg::onClose( wxCloseEvent& event ) {
  EndModal(0);
}

bool XmlScriptDlg::validateXML(bool okmessage) {
  char* xml = StrOp.dup(m_XML->GetValue().mb_str(wxConvUTF8));
  long erroffset = 0;
  long scriptlen = StrOp.len(xml);
  long textsize  = m_XML->GetLastPosition();
  iODoc doc = DocOp.parseErr(xml, (int*)&erroffset);
  if( doc == NULL ) {
    wxMessageDialog( this, wxT("Not well formed XML. Error offset is ") + wxString::Format(wxT("%d of %d(%d)"),
        (int)erroffset, (int)scriptlen, (int)textsize ), wxT("Rocrail"), wxOK | wxICON_EXCLAMATION ).ShowModal();

    m_XML->Clear();
    bool backRed = false;
    for( int i = 0; i < scriptlen; i++ ) {
      if( i == erroffset ) {
        m_XML->SetDefaultStyle(wxTextAttr(*wxBLACK, Base::getRed()));
        backRed = true;
      }
      char str[2];
      str[0] = xml[i];
      str[1] = '\0';
      m_XML->AppendText(wxString(str,wxConvUTF8));
      if( backRed && xml[i] == '\n' ) {
        m_XML->SetDefaultStyle(wxTextAttr(*wxBLACK, *wxWHITE));
        backRed = false;
      }
    }

    StrOp.free(xml);
    return false;
  }
  iONode root = DocOp.getRootNode(doc);
  if( root == NULL ) {
    DocOp.base.del(doc);
    wxMessageDialog( this, wxT("Root node is missing."), wxT("Rocrail"), wxOK | wxICON_EXCLAMATION ).ShowModal();
    StrOp.free(xml);
    return false;
  }
  NodeOp.base.del(root);
  if( okmessage )
    wxMessageDialog( this, wxT("XMLScript is well formed."), wxT("Rocrail"), wxOK ).ShowModal();

  m_XML->Clear();
  m_XML->SetDefaultStyle(wxTextAttr(*wxBLACK, *wxWHITE));
  m_XML->SetValue( wxString(xml,wxConvUTF8) );
  StrOp.free(xml);
  return true;
}

void XmlScriptDlg::onValidate( wxCommandEvent& event ) {
  validateXML();
}

void XmlScriptDlg::onHelp( wxCommandEvent& event )
{
  wxGetApp().openLink( "xmlscripting" );
}

void XmlScriptDlg::onSave( wxCommandEvent& event )
{
  if( validateXML(false) ) {
    iONode cmd = NodeOp.inst( wDataReq.name(), NULL, ELEMENT_NODE );
    wDataReq.setcmd( cmd, wDataReq.writexmlscript );
    wDataReq.setfilename( cmd, wDataReq.getfilename(m_Data) );
    wDataReq.setdata( cmd, m_XML->GetValue().mb_str(wxConvUTF8) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
    EndModal( wxID_OK );
  }
}
