/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifndef LBSERIAL_H_
#define LBSERIAL_H_

Boolean lbserialConnect( obj inst );
void  lbserialDisconnect( obj inst );

int lbserialRead ( obj inst, unsigned char *msg );
Boolean lbserialWrite( obj inst, unsigned char *msg, int len );
Boolean lbserialAvailable( obj inst );

#endif /*LBSERIAL_H_*/
