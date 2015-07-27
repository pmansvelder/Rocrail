/*
 Rocrail - Model Railroad Software
 Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net
 All rights reserved.
*/

#ifndef __issuedlg__
#define __issuedlg__

#include "issuedlggen.h"

#include "rocs/public/node.h"

//// end generated include

/** Implementing IssueDlgGen */
class IssueDlg : public IssueDlgGen
{
  iONode m_Issue;
	protected:
		// Handlers for IssueDlgGen events.
		void onCancel( wxCommandEvent& event );
		void onOK( wxCommandEvent& event );
		void onHelp( wxCommandEvent& event );
	public:
		/** Constructor */
		IssueDlg( wxWindow* parent );
		iONode getIssue() {return m_Issue;}
	//// end generated class members
	
};

#endif // __issuedlg__
