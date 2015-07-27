/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifndef LNMASTER_H_
#define LNMASTER_H_

#include "rocdigs/public/loconet.h"

void lnmasterThread( void* threadinst );
int lnLocoAddr(int addrH, int addrL); 

#endif /*LNMASTER_H_*/
