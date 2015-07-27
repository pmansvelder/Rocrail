/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef RN_TCP_H_
#define RN_TCP_H_

Boolean rnTcpConnect( obj inst );
void  rnTcpDisconnect( obj inst );

int rnTcpRead ( obj inst, unsigned char *msg );
Boolean rnTcpWrite( obj inst, unsigned char *msg, int len );
Boolean rnTcpAvailable( obj inst );

#endif /* RN_TCP_H_ */
