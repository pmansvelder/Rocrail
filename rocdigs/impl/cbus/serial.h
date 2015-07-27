/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifndef CBUSSERIAL_H_
#define CBUSSERIAL_H_

Boolean serialConnect( obj inst );
void  serialDisconnect( obj inst );

Boolean serialRead ( obj inst, unsigned char *msg, int len );
Boolean serialWrite( obj inst, unsigned char *msg, int len );
Boolean serialAvailable( obj inst );

#endif /*CBUSSERIAL_H_*/
