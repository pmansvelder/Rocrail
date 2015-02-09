///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "dmxdlggen.h"

///////////////////////////////////////////////////////////////////////////

dmxdlggen::dmxdlggen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer11;
	fgSizer11 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer11->AddGrowableCol( 1 );
	fgSizer11->SetFlexibleDirection( wxBOTH );
	fgSizer11->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labIID = new wxStaticText( this, wxID_ANY, wxT("IID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labIID->Wrap( -1 );
	fgSizer11->Add( m_labIID, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_IID = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_IID, 0, wxALL|wxEXPAND, 5 );
	
	m_labHost = new wxStaticText( this, wxID_ANY, wxT("Hostname"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labHost->Wrap( -1 );
	fgSizer11->Add( m_labHost, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_Host = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Host->SetMinSize( wxSize( 160,-1 ) );
	
	fgSizer11->Add( m_Host, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_labFrameRate = new wxStaticText( this, wxID_ANY, wxT("Frame rate"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labFrameRate->Wrap( -1 );
	fgSizer11->Add( m_labFrameRate, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_FrameRate = new wxSpinCtrl( this, wxID_ANY, wxT("500"), wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 100, 10000, 500 );
	fgSizer2->Add( m_FrameRate, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_labFrameRateMS = new wxStaticText( this, wxID_ANY, wxT("ms"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labFrameRateMS->Wrap( -1 );
	fgSizer2->Add( m_labFrameRateMS, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	fgSizer11->Add( fgSizer2, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer9->Add( fgSizer11, 1, wxEXPAND, 5 );
	
	m_MapWhite = new wxCheckBox( this, wxID_ANY, wxT("Map white"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_MapWhite, 0, wxALL, 5 );
	
	m_stdButtons = new wxStdDialogButtonSizer();
	m_stdButtonsOK = new wxButton( this, wxID_OK );
	m_stdButtons->AddButton( m_stdButtonsOK );
	m_stdButtonsCancel = new wxButton( this, wxID_CANCEL );
	m_stdButtons->AddButton( m_stdButtonsCancel );
	m_stdButtonsHelp = new wxButton( this, wxID_HELP );
	m_stdButtons->AddButton( m_stdButtonsHelp );
	m_stdButtons->Realize();
	bSizer9->Add( m_stdButtons, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer9 );
	this->Layout();
	bSizer9->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_stdButtonsCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( dmxdlggen::onCancel ), NULL, this );
	m_stdButtonsHelp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( dmxdlggen::onHelp ), NULL, this );
	m_stdButtonsOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( dmxdlggen::onOK ), NULL, this );
}

dmxdlggen::~dmxdlggen()
{
	// Disconnect Events
	m_stdButtonsCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( dmxdlggen::onCancel ), NULL, this );
	m_stdButtonsHelp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( dmxdlggen::onHelp ), NULL, this );
	m_stdButtonsOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( dmxdlggen::onOK ), NULL, this );
	
}
