/*
 Rocrail - Model Railroad Software
 Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net
 All rights reserved.
*/



#include "aboxdlg.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include <wx/filedlg.h>
#include <wx/mimetype.h>

#include "rocview/public/guiapp.h"

#include "rocrail/wrapper/public/DataReq.h"
#include "rocrail/wrapper/public/DirEntry.h"
#include "rocrail/wrapper/public/FileEntry.h"

ABoxDlg::ABoxDlg( wxWindow* parent ):AboxDlgGen( parent )
{
  m_StubList = ListOp.inst();
  initLabels();
}

ABoxDlg::~ABoxDlg() {
  clearStubList();
}

void ABoxDlg::clearStubList() {
  for(int i = 0; i < ListOp.size(m_StubList); i++) {
    iONode stub = (iONode)ListOp.get(m_StubList, i);
    NodeOp.base.del(stub);
  }
  ListOp.clear(m_StubList);
}

void ABoxDlg::initLabels() {
  m_Find->SetLabel( wxGetApp().getMsg( "find" ) );
  m_Add->SetLabel( wxGetApp().getMsg( "add" ) );
  m_SelectFile->SetLabel( wxGetApp().getMsg( "select" ) + wxT("...") );
  m_labText->SetLabel( wxGetApp().getMsg( "text" ) );
  m_labCategory->SetLabel( wxGetApp().getMsg( "category" ) );

  m_Stubs->InsertColumn(0, wxGetApp().getMsg( "file" ), wxLIST_FORMAT_LEFT );
  m_Stubs->InsertColumn(1, wxGetApp().getMsg( "category" ), wxLIST_FORMAT_LEFT );
  m_Stubs->InsertColumn(2, wxGetApp().getMsg( "text" ), wxLIST_FORMAT_LEFT );
}

void ABoxDlg::onFind( wxCommandEvent& event ) {
  iONode cmd = NodeOp.inst( wDataReq.name(), NULL, ELEMENT_NODE );
  wDataReq.setcmd( cmd, wDataReq.abox_find );
  wDataReq.settext( cmd, m_FindText->GetValue().mb_str(wxConvUTF8) );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void ABoxDlg::onSelectFile( wxCommandEvent& event ) {
  wxFileDialog* fdlg = new wxFileDialog(this);
  if( fdlg->ShowModal() == wxID_OK ) {
    m_Filename->SetValue(fdlg->GetPath());
  }
  fdlg->Destroy();
  Raise();
}

void ABoxDlg::onAdd( wxCommandEvent& event ) {
  iONode cmd = NodeOp.inst( wDataReq.name(), NULL, ELEMENT_NODE );
  wDataReq.setcmd( cmd, wDataReq.abox_addlink );

  iONode direntry = NodeOp.inst(wDirEntry.name(), cmd, ELEMENT_NODE );
  NodeOp.addChild(cmd, direntry);
  iONode fileentry = NodeOp.inst(wFileEntry.name(), cmd, ELEMENT_NODE );
  NodeOp.addChild(direntry, fileentry);

  wFileEntry.setfname( fileentry, m_Filename->GetValue().mb_str(wxConvUTF8) );
  wFileEntry.settext( fileentry, m_Text->GetValue().mb_str(wxConvUTF8) );
  wFileEntry.setcategory( fileentry, m_Category->GetValue().mb_str(wxConvUTF8) );

  char* s = NodeOp.base.toString(cmd);
  TraceOp.trc( "aboxdlg", TRCLEVEL_INFO, __LINE__, 9999, "add: %s", s );
  StrOp.free(s);

  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);

  m_Filename->SetValue(wxT(""));
  m_Text->SetValue(wxT(""));
  m_Category->SetValue(wxT(""));

}


void ABoxDlg::onStubActivated( wxListEvent& event ) {
  int index = event.GetIndex();
  iONode stub = (iONode)m_Stubs->GetItemData(index);
  wxMimeTypesManager manager;
  wxFileType *filetype=manager.GetFileTypeFromExtension(wxString(StrOp.getExtension(NodeOp.getStr(stub, "path", "-")),wxConvUTF8));
  wxString command=filetype->GetOpenCommand(wxT("\"")+wxString(NodeOp.getStr(stub, "path", "-"),wxConvUTF8)+wxT("\""));
  wxExecute(command);
}

void ABoxDlg::onStubSelected( wxListEvent& event ) {

}

void ABoxDlg::event(iONode node) {
  m_Stubs->DeleteAllItems();
  clearStubList();

  char* s = NodeOp.base.toString(node);
  TraceOp.trc( "aboxdlg", TRCLEVEL_INFO, __LINE__, 9999, "found [%s]", s );
  StrOp.free(s);

  int idx = 0;
  iONode stub = NodeOp.findNode( node, "stub");
  while( stub != NULL ) {
    m_Stubs->InsertItem( idx, wxString(NodeOp.getStr(stub, "path", "-"),wxConvUTF8) );
    m_Stubs->SetItem( idx, 1, wxString(NodeOp.getStr(stub, "category", "-"),wxConvUTF8) );
    m_Stubs->SetItem( idx, 2, wxString(NodeOp.getStr(stub, "text", "-"),wxConvUTF8) );
    iONode clone = (iONode)NodeOp.base.clone(stub);
    m_Stubs->SetItemPtrData(idx, (wxUIntPtr)clone);
    ListOp.add(m_StubList, (obj)clone);
    idx++;
    stub = NodeOp.findNextNode( node, stub);
  }

  // resize
  for( int n = 0; n < 3; n++ ) {
    m_Stubs->SetColumnWidth(n, wxLIST_AUTOSIZE_USEHEADER);
    int autoheadersize = m_Stubs->GetColumnWidth(n);
    m_Stubs->SetColumnWidth(n, wxLIST_AUTOSIZE);
    int autosize = m_Stubs->GetColumnWidth(n);
    if(autoheadersize > autosize )
      m_Stubs->SetColumnWidth(n, wxLIST_AUTOSIZE_USEHEADER);
  }


}

