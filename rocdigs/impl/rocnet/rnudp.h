/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef RN_UDP_H_
#define RN_UDP_H_

Boolean rnUDPConnect( obj inst );
void  rnUDPDisconnect( obj inst );

int rnUDPRead ( obj inst, unsigned char *msg );
Boolean rnUDPWrite( obj inst, unsigned char *msg, int len );
Boolean rnUDPAvailable( obj inst );

#endif /* RN_UDP_H_ */
