/* Rocrail - Model Railroad Software Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net All rights reserved.*/#ifndef __xmlscriptdlg__#define __xmlscriptdlg__#include "rocs/public/node.h"/**@fileSubclass of xmlscriptdlggen, which is generated by wxFormBuilder.*/#include "xmlscriptdlggen.h"//// end generated include/** Implementing xmlscriptdlggen */class XmlScriptDlg : public xmlscriptdlggen{  iONode m_Data;  bool validateXML(bool okmessage=true);  void initLabels();	protected:		// Handlers for xmlscriptdlggen events.		void onCancel( wxCommandEvent& event );		void onClose( wxCloseEvent& event );		void onHelp( wxCommandEvent& event );		void onSave( wxCommandEvent& event );		void onValidate( wxCommandEvent& event );	public:		/** Constructor */		XmlScriptDlg( wxWindow* parent, iONode node );	//// end generated class members	};#endif // __xmlscriptdlg__