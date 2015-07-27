/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef ZIMOCAN_UDP_H_
#define ZIMOCAN_UDP_H_

Boolean UDPConnect( obj inst );
void  UDPDisconnect( obj inst );

int UDPRead ( obj inst, unsigned char *msg );
Boolean UDPWrite( obj inst, unsigned char *msg, int len );
Boolean UDPAvailable( obj inst );

#endif /* ZIMOCAN_UDP_H_ */
