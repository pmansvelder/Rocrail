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
#include "rocview/wrapper/public/Gui.h"
#include "rocview/wrapper/public/ABox.h"

#include "rocrail/wrapper/public/DataReq.h"
#include "rocrail/wrapper/public/DirEntry.h"
#include "rocrail/wrapper/public/FileEntry.h"

#include "rocs/public/strtok.h"

static bool ms_Sort = true;

ABoxDlg::ABoxDlg( wxWindow* parent, const char* text ):AboxDlgGen( parent )
{
  m_StubList = ListOp.inst();
  m_SelectedStub = wxNOT_FOUND;
  m_ReadOnly = false;
  m_SortCol = 0;
  m_AddedFilename[0] = '\0';
  m_AddedUID[0] = '\0';
  m_AddedPart = -1;

  m_Ini = wGui.getabox( wxGetApp().getIni() );
  if( m_Ini == NULL ) {
    m_Ini = NodeOp.inst(wABox.name(), wxGetApp().getIni(), ELEMENT_NODE);
    NodeOp.addChild( wxGetApp().getIni(), m_Ini );
  }

  m_FindInText->SetValue(wABox.isfindintext(m_Ini)?true:false);
  m_FindInCategory->SetValue(wABox.isfindincategory(m_Ini)?true:false);
  m_FindInFilename->SetValue(wABox.isfindinfilename(m_Ini)?true:false);
  m_ShowPath->SetValue(wABox.isshowpath(m_Ini)?true:false);

  m_Open->Enable(false);
  m_Modify->Enable(false);
  m_Delete->Enable(false);
  m_Link->Enable(false); // only links are supported

  initLabels();

  iONode cmd = NodeOp.inst( wDataReq.name(), NULL, ELEMENT_NODE );
  wDataReq.setcmd( cmd, wDataReq.abox_getcategories );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);

  if( text != NULL ) {
    m_FindText->SetValue( wxString(text,wxConvUTF8) );
    doFind(text);
  }
  else
    doFind("");
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
  m_FindInText->SetLabel( wxGetApp().getMsg( "text" ) );
  m_FindInCategory->SetLabel( wxGetApp().getMsg( "category" ) );
  m_FindInFilename->SetLabel( wxGetApp().getMsg( "filename" ) );
  m_Add->SetLabel( wxGetApp().getMsg( "add" ) );
  m_Open->SetLabel( wxGetApp().getMsg( "open" ) );
  m_Modify->SetLabel( wxGetApp().getMsg( "modify" ) );
  m_Delete->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_labFile->SetLabel( wxGetApp().getMsg( "file" ) );
  m_labText->SetLabel( wxGetApp().getMsg( "text" ) );
  m_labCategory->SetLabel( wxGetApp().getMsg( "category" ) );
  m_labResultText->SetLabel( wxGetApp().getMsg( "text" ) );
  m_labResultNote->SetLabel( wxGetApp().getMsg( "note" ) );
  m_Link->SetLabel( wxGetApp().getMsg( "link" ) );
  m_ShowPath->SetLabel( wxGetApp().getMsg( "showpath" ) );

  m_FindBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "find" ) );
  m_UploadBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "upload" ) );
  m_ResultBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "result" ) );

  m_Stubs->InsertColumn(0, wxGetApp().getMsg( "file" ), wxLIST_FORMAT_LEFT );
  m_Stubs->InsertColumn(1, wxGetApp().getMsg( "category" ), wxLIST_FORMAT_LEFT );
  m_Stubs->InsertColumn(2, wxGetApp().getMsg( "text" ), wxLIST_FORMAT_LEFT );
  m_Stubs->InsertColumn(3, wxT( "UID" ), wxLIST_FORMAT_LEFT );
}

void ABoxDlg::doFind( const char* text ) {
  wABox.setfindintext(m_Ini, m_FindInText->IsChecked()?True:False);
  wABox.setfindincategory(m_Ini, m_FindInCategory->IsChecked()?True:False);
  wABox.setfindinfilename(m_Ini, m_FindInFilename->IsChecked()?True:False);

  iONode cmd = NodeOp.inst( wDataReq.name(), NULL, ELEMENT_NODE );
  wDataReq.setcmd( cmd, wDataReq.abox_find );
  wDataReq.settext( cmd, text );
  wDataReq.setfindintext( cmd, m_FindInText->IsChecked()?True:False );
  wDataReq.setfindincategory( cmd, m_FindInCategory->IsChecked()?True:False );
  wDataReq.setfindinfilename( cmd, m_FindInFilename->IsChecked()?True:False );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void ABoxDlg::onFind( wxCommandEvent& event ) {
  doFind(m_FindText->GetValue().mb_str(wxConvUTF8));
}

void ABoxDlg::onFindTextEnter( wxCommandEvent& event ) {
  doFind(m_FindText->GetValue().mb_str(wxConvUTF8));
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
  if( m_Filename->GetValue().IsEmpty() || m_Text->GetValue().IsEmpty() || m_Category->GetValue().IsEmpty() ) {
    // Upload criteria missing.
    int action = wxMessageDialog( this, wxGetApp().getMsg("missingcriteria"), _T("Rocrail"), wxOK | wxICON_EXCLAMATION ).ShowModal();
    return;
  }

  iONode cmd = NodeOp.inst( wDataReq.name(), NULL, ELEMENT_NODE );
  wDataReq.setcmd( cmd, wDataReq.abox_addlink );

  iONode direntry = NodeOp.inst(wDirEntry.name(), cmd, ELEMENT_NODE );
  NodeOp.addChild(cmd, direntry);
  iONode fileentry = NodeOp.inst(wFileEntry.name(), cmd, ELEMENT_NODE );
  NodeOp.addChild(direntry, fileentry);

  wFileEntry.setfname( fileentry, m_Filename->GetValue().mb_str(wxConvUTF8) );

  // save the file path in case it must be uploaded
  m_AddedUID[0] = '\0';
  if( !m_Link->IsChecked() )
    StrOp.copy( m_AddedFilename, wFileEntry.getfname(fileentry) );
  else
    m_AddedFilename[0] = '\0';

  wFileEntry.settext( fileentry, m_Text->GetValue().mb_str(wxConvUTF8) );
  wFileEntry.setcategory( fileentry, m_Category->GetValue().mb_str(wxConvUTF8) );

  char* s = NodeOp.base.toString(cmd);
  TraceOp.trc( "aboxdlg", TRCLEVEL_INFO, __LINE__, 9999, "add: %s", s );
  StrOp.free(s);

  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);

  m_FindText->SetValue(m_Text->GetValue());

  m_Filename->SetValue(wxT(""));
  m_Text->SetValue(wxT(""));
  m_Category->SetValue(wxT(""));

  onFind(event);

  cmd = NodeOp.inst( wDataReq.name(), NULL, ELEMENT_NODE );
  wDataReq.setcmd( cmd, wDataReq.abox_getcategories );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void ABoxDlg::openStub() {
  iONode stub = (iONode)m_Stubs->GetItemData(m_SelectedStub);
  wxMimeTypesManager manager;
  wxFileType *filetype=manager.GetFileTypeFromExtension(wxString(StrOp.getExtension(NodeOp.getStr(stub, "path", "-")),wxConvUTF8));
  wxString command=filetype->GetOpenCommand(wxString(NodeOp.getStr(stub, "path", "-"),wxConvUTF8));
  TraceOp.trc( "aboxdlg", TRCLEVEL_INFO, __LINE__, 9999, "execute [%s]", (const char*)command.mb_str(wxConvUTF8) );
  wxExecute(command);
}

void ABoxDlg::showStub() {
  iONode stub = (iONode)m_Stubs->GetItemData(m_SelectedStub);
  m_ResultText->SetValue(wxString(NodeOp.getStr(stub, "text", "-"),wxConvUTF8));
  m_ResultNote->SetValue(wxString(NodeOp.getStr(stub, "note", ""),wxConvUTF8));
}

void ABoxDlg::onStubActivated( wxListEvent& event ) {
  m_SelectedStub = event.GetIndex();
  m_Open->Enable(true);
  m_Modify->Enable(!m_ReadOnly);
  m_Delete->Enable(!m_ReadOnly);
  openStub();
}

void ABoxDlg::onStubSelected( wxListEvent& event ) {
  m_SelectedStub = event.GetIndex();
  m_Open->Enable(true);
  m_Modify->Enable(!m_ReadOnly);
  m_Delete->Enable(!m_ReadOnly);
  showStub();
}


static int __sortPath(obj* _a, obj* _b) {
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = NodeOp.getStr(a, "path", "-");
    const char* idB = NodeOp.getStr(b, "path", "-");
    return ms_Sort ? strcmp( idA, idB ):strcmp( idB, idA );
}
static int __sortFilename(obj* _a, obj* _b) {
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = FileOp.ripPath(NodeOp.getStr(a, "path", "-"));
    const char* idB = FileOp.ripPath(NodeOp.getStr(b, "path", "-"));
    return ms_Sort ? strcmp( idA, idB ):strcmp( idB, idA );
}
static int __sortCategory(obj* _a, obj* _b) {
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = NodeOp.getStr(a, "category", "-");
    const char* idB = NodeOp.getStr(b, "category", "-");
    return ms_Sort ? strcmp( idA, idB ):strcmp( idB, idA );
}
static int __sortText(obj* _a, obj* _b) {
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = NodeOp.getStr(a, "text", "-");
    const char* idB = NodeOp.getStr(b, "text", "-");
    return ms_Sort ? strcmp( idA, idB ):strcmp( idB, idA );
}
static int __sortUID(obj* _a, obj* _b) {
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = NodeOp.getStr(a, "uid", "-");
    const char* idB = NodeOp.getStr(b, "uid", "-");
    return ms_Sort ? strcmp( idA, idB ):strcmp( idB, idA );
}

void ABoxDlg::initResult() {
  m_Stubs->DeleteAllItems();
  m_ResultText->SetValue(wxT(""));
  m_ResultNote->SetValue(wxT(""));

  iOList list = ListOp.inst();
  int listSize = ListOp.size(m_StubList);
  for( int idx = 0; idx < listSize; idx++ ) {
    ListOp.add(list, ListOp.get(m_StubList, idx));
  }

  if( m_SortCol == 1 ) {
    ListOp.sort(list, &__sortCategory);
  }
  else if( m_SortCol == 2 ) {
    ListOp.sort(list, &__sortText);
  }
  else if( m_SortCol == 3 ) {
    ListOp.sort(list, &__sortUID);
  }
  else {
    if( m_ShowPath->IsChecked() )
      ListOp.sort(list, &__sortPath);
    else
      ListOp.sort(list, &__sortFilename);
  }

  listSize = ListOp.size(list);
  for( int idx = 0; idx < listSize; idx++ ) {
    iONode stub = (iONode)ListOp.get( list, idx);
    if( m_ShowPath->IsChecked() )
      m_Stubs->InsertItem( idx, wxString(NodeOp.getStr(stub, "path", "-"),wxConvUTF8) );
    else
      m_Stubs->InsertItem( idx, wxString(FileOp.ripPath(NodeOp.getStr(stub, "path", "-")),wxConvUTF8) );
    m_Stubs->SetItem( idx, 1, wxString(NodeOp.getStr(stub, "category", "-"),wxConvUTF8) );
    m_Stubs->SetItem( idx, 2, wxString(NodeOp.getStr(stub, "text", "-"),wxConvUTF8) );
    m_Stubs->SetItem( idx, 3, wxString(NodeOp.getStr(stub, "uid", "-"),wxConvUTF8) );
    m_Stubs->SetItemPtrData(idx, (wxUIntPtr)stub);
  }

  ListOp.base.del(list);

  // resize
  for( int n = 0; n < 4; n++ ) {
    m_Stubs->SetColumnWidth(n, wxLIST_AUTOSIZE_USEHEADER);
    int autoheadersize = m_Stubs->GetColumnWidth(n);
    m_Stubs->SetColumnWidth(n, wxLIST_AUTOSIZE);
    int autosize = m_Stubs->GetColumnWidth(n);
    if(autoheadersize > autosize )
      m_Stubs->SetColumnWidth(n, wxLIST_AUTOSIZE_USEHEADER);
  }
}

void ABoxDlg::event(iONode node) {
  if( wDataReq.getcmd(node) == wDataReq.abox_addlink ) {
    const char* uid = wDataReq.getid(node);
    iONode direntry = wDataReq.getdirentry(node);
    if( direntry != NULL && wDirEntry.getfileentry(direntry) != NULL ) {
      iONode fileentry = wDirEntry.getfileentry(direntry);
      TraceOp.trc( "aboxdlg", TRCLEVEL_INFO, __LINE__, 9999, "uid=%s for [%s]", uid, wFileEntry.getfname(fileentry) );
      /*
      char* s = StrOp.fmt("%s:\nUID=%s\n%s=%s", wxGetApp().getCMsg("upload"), uid, wxGetApp().getCMsg("file"), wFileEntry.getfname(fileentry) );
      int action = wxMessageDialog( this, wxString(s,wxConvUTF8), _T("Rocrail"), wxOK ).ShowModal();
      StrOp.free(s);
      */
    }
  }

  else if( wDataReq.getcmd(node) == wDataReq.abox_filedata ) {

  }

  else if( wDataReq.getcmd(node) == wDataReq.abox_getcategories ) {
    m_ReadOnly = wDataReq.isreadonly(node)?true:false;
    m_Add->Enable(!m_ReadOnly);
    m_Category->Clear();
    m_FindText->Clear();
    iOStrTok tok = StrTokOp.inst( wDataReq.getcategory( node ), ',' );
    while( StrTokOp.hasMoreTokens(tok) ) {
      const char* category = StrTokOp.nextToken( tok );
      m_Category->Append( wxString(category,wxConvUTF8) );
      m_FindText->Append( wxString(category,wxConvUTF8) );
    }
  }

  else if( wDataReq.getcmd(node) == wDataReq.abox_find ) {
    m_SelectedStub = wxNOT_FOUND;
    m_Open->Enable(false);
    m_Modify->Enable(false);
    m_Delete->Enable(false);
    m_Stubs->DeleteAllItems();
    clearStubList();

    char* s = NodeOp.base.toString(node);
    TraceOp.trc( "aboxdlg", TRCLEVEL_INFO, __LINE__, 9999, "found [%s]", s );
    StrOp.free(s);

    iONode stub = NodeOp.findNode( node, "stub");
    while( stub != NULL ) {
      iONode clone = (iONode)NodeOp.base.clone(stub);
      ListOp.add(m_StubList, (obj)clone);
      stub = NodeOp.findNextNode( node, stub);
    }
    initResult();
  }

}

void ABoxDlg::onHelp( wxCommandEvent& event ) {
  wxGetApp().openLink( "abox:archivebox" );
}

void ABoxDlg::onOK( wxCommandEvent& event ) {
  EndModal( wxID_OK );
}

void ABoxDlg::onOpen( wxCommandEvent& event ) {
  if( m_SelectedStub != wxNOT_FOUND ) {
    openStub();
  }
}

void ABoxDlg::onDelete( wxCommandEvent& event ) {
  if( m_SelectedStub != wxNOT_FOUND ) {
    int action = wxMessageDialog( this, wxGetApp().getMsg("removewarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
    if( action == wxID_NO )
      return;
    iONode stub = (iONode)m_Stubs->GetItemData(m_SelectedStub);
    iONode cmd = NodeOp.inst( wDataReq.name(), NULL, ELEMENT_NODE );
    wDataReq.setcmd( cmd, wDataReq.abox_deletelink );
    NodeOp.addChild(cmd, (iONode)NodeOp.base.clone(stub));
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);

    onFind(event);
  }
}

void ABoxDlg::onStubCol( wxListEvent& event ) {
  if(m_SortCol == event.GetColumn())
    ms_Sort = !ms_Sort;
  else
    ms_Sort = true;

  m_SortCol = event.GetColumn();
  initResult();
}


void ABoxDlg::onModify( wxCommandEvent& event ) {
  if( m_SelectedStub != wxNOT_FOUND ) {
    iONode stub = (iONode)m_Stubs->GetItemData(m_SelectedStub);
    iONode cmd = NodeOp.inst( wDataReq.name(), NULL, ELEMENT_NODE );
    wDataReq.setcmd( cmd, wDataReq.abox_modifylink );
    iONode clone = (iONode)NodeOp.base.clone(stub);
    NodeOp.setStr(clone, "text", m_ResultText->GetValue().mb_str(wxConvUTF8) );
    NodeOp.setStr(clone, "note", m_ResultNote->GetValue().mb_str(wxConvUTF8) );
    NodeOp.addChild(cmd, clone);
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);

    onFind(event);
  }
}

void ABoxDlg::onShowPath( wxCommandEvent& event ) {
  wABox.setshowpath(m_Ini, m_ShowPath->IsChecked()?True:False);
  initResult();
}


