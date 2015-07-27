/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef ZIMOCAN_SERIAL_H_
#define ZIMOCAN_SERIAL_H_

Boolean SerialConnect( obj inst );
void  SerialDisconnect( obj inst );

int SerialRead ( obj inst, unsigned char *msg );
Boolean SerialWrite( obj inst, unsigned char *msg, int len );
Boolean SerialAvailable( obj inst );


#endif /* ZIMOCAN_SERIAL_H_ */
