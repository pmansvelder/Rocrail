///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __aboxdlggen__
#define __aboxdlggen__

#include <wx/string.h>
#include <wx/button.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/listctrl.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AboxDlgGen
///////////////////////////////////////////////////////////////////////////////
class AboxDlgGen : public wxDialog 
{
	private:
	
	protected:
		wxButton* m_Find;
		wxTextCtrl* m_FindText;
		wxStaticLine* m_staticline1;
		wxButton* m_SelectFile;
		wxTextCtrl* m_Filename;
		wxStaticText* m_staticText1;
		wxStaticText* m_labCategory;
		wxStaticText* m_labText;
		wxButton* m_Add;
		wxTextCtrl* m_Category;
		wxTextCtrl* m_Text;
		wxListCtrl* m_Stubs;
		wxStdDialogButtonSizer* m_StdButtons;
		wxButton* m_StdButtonsOK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onFind( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSelectFile( wxCommandEvent& event ) { event.Skip(); }
		virtual void onAdd( wxCommandEvent& event ) { event.Skip(); }
		virtual void onStubActivated( wxListEvent& event ) { event.Skip(); }
		virtual void onStubSelected( wxListEvent& event ) { event.Skip(); }
		
	
	public:
		
		AboxDlgGen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("ArchiveBox"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~AboxDlgGen();
	
};

#endif //__aboxdlggen__
