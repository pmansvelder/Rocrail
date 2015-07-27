/*
 Rocrail - Model Railroad Software
 Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net
 All rights reserved.
*/

#include "issuedlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocs/public/trace.h"
#include "rocs/public/str.h"
#include "rocview/public/guiapp.h"

#include "rocrail/wrapper/public/Issue.h"

IssueDlg::IssueDlg( wxWindow* parent ):IssueDlgGen( parent )
{
  m_Issue = NULL;
  SetTitle(wxGetApp().getMsg( "issue" ));
  m_labSubject->SetLabel( wxGetApp().getMsg( "subject" ) );
  // Buttons
  m_StdButtonOK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_StdButtonCancel->SetLabel( wxGetApp().getMsg( "cancel" ) );

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
}

void IssueDlg::onCancel( wxCommandEvent& event )
{
  EndModal( 0 );
}

void IssueDlg::onOK( wxCommandEvent& event )
{
  m_Issue = NodeOp.inst( wIssue.name(), NULL, ELEMENT_NODE );
  wIssue.setsubject(m_Issue, m_Subject->GetValue().mb_str(wxConvUTF8));
  wIssue.setdesc(m_Issue, m_Description->GetValue().mb_str(wxConvUTF8));
  /*
  wIssue.setini(m_Issue, m_InclIni->IsChecked() ? True:False);
  wIssue.settrc(m_Issue, m_InclTrc->IsChecked() ? True:False);
  wIssue.setstat(m_Issue, m_InclStat->IsChecked() ? True:False);
  wIssue.setxml(m_Issue, m_InclXML->IsChecked() ? True:False);
  */
  EndModal( wxID_OK );
}

void IssueDlg::onHelp( wxCommandEvent& event ) {
  wxGetApp().openLink( "issue" );
}
