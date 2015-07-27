/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifdef __GNUG__
    #pragma implementation "rocgui.cpp"
    #pragma interface "rocgui.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


#include "rocs/public/node.h"

#include "rocview/public/basepanel.h"

/**
 * Abstract BasePanel class to initialize the wxScrolledWindow.
 */

BasePanel::BasePanel(wxWindow* parent, long style )
             :wxScrolledWindow( parent, -1, wxDefaultPosition, wxDefaultSize, style )
{
}
