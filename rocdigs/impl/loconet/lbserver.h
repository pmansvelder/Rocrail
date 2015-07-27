/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifndef LBSERVER_H_
#define LBSERVER_H_

#include "rocs/public/socket.h"


Boolean lbserverConnect( obj inst );
void  lbserverDisconnect( obj inst );

int lbserverRead ( obj inst, unsigned char *msg );
Boolean lbserverWrite( obj inst, unsigned char *msg, int len );
Boolean lbserverAvailable( obj inst );

#endif /*LBSERVER_H_*/
