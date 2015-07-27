/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifndef CBUSTCP_H_
#define CBUSTCP_H_

Boolean tcpConnect( obj inst );
void  tcpDisconnect( obj inst );

Boolean tcpRead ( obj inst, unsigned char *msg, int len );
Boolean tcpWrite( obj inst, unsigned char *msg, int len );
Boolean tcpAvailable( obj inst );

#endif /*CBUSTCP_H_*/
