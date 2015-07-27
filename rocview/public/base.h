/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifndef __ROCRAIL_rocgui_BASE_H
#define __ROCRAIL_rocgui_BASE_H


#include "wx/wx.h"


class Base
{
public:
  static wxColor getWhite ();
  static wxColor getBlack ();
  static wxColor getRed   ();
  static wxColor getOrange();
  static wxColor getTurquoise();
  static wxColor getYellow();
  static wxColor getYellow2();
  static wxColor getGreen ();
  static wxColor getBlue  ();
  static wxColor getGrey  ();
  static wxColor getDarkGrey  ();
  static wxColor getNoneActiveColor();
  static wxColor getResColor();
  static wxColor getRes2Color();
};

#endif
