/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef RN_SERIAL_H_
#define RN_SERIAL_H_

Boolean rnSerialConnect( obj inst );
void  rnSerialDisconnect( obj inst );

int rnSerialRead ( obj inst, unsigned char *msg );
Boolean rnSerialWrite( obj inst, unsigned char *msg, int len );
Boolean rnSerialAvailable( obj inst );


#endif /* RN_SERIAL_H_ */
