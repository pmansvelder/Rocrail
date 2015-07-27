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
#include "rocs/public/str.h"
#include "rocs/public/trace.h"
#include "rocs/public/file.h"

#include "rocview/public/base.h"


wxColor Base::getWhite (){ return wxColor( 255, 255, 255 ); }
wxColor Base::getBlack (){ return wxColor(   0,   0,   0 ); }
wxColor Base::getRed   (){ return wxColor( 255, 200, 200 ); }
wxColor Base::getOrange(){ return wxColor( 255, 127, 0 ); }
wxColor Base::getTurquoise(){ return wxColor( 127, 224, 208 ); }
wxColor Base::getYellow(){ return wxColor( 255, 255,   0 ); }
wxColor Base::getYellow2(){ return wxColor( 255, 255, 100 ); }
wxColor Base::getGreen (){ return wxColor( 200, 255, 200 ); }
wxColor Base::getBlue  (){ return wxColor( 200, 200, 255 ); }
wxColor Base::getGrey  (){ return wxColor( 225, 225, 225 ); }
wxColor Base::getDarkGrey  (){ return wxColor( 200, 200, 200 ); }
wxColor Base::getNoneActiveColor(){ return wxColor( 240, 200, 200 ); }
wxColor Base::getResColor(){ return wxColor( 255, 255, 200 ); }
wxColor Base::getRes2Color(){ return wxColor( 255, 255, 170 ); }
