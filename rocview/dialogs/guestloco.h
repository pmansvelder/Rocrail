/*
 Rocrail - Model Railroad Software
 Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net
 All rights reserved.
*/
#ifndef __guestloco__
#define __guestloco__

#include "guestlocogen.h"


/** Implementing GuestLocoGen */
class GuestLoco : public GuestLocoGen
{
	public:
  /** Constructor */
  GuestLoco( wxWindow* parent );
  void onCancel( wxCommandEvent& event );
  void onOK( wxCommandEvent& event );
  void onHelp( wxCommandEvent& event );
};

#endif // __guestloco__
