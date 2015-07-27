/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef LBTCP_H_
#define LBTCP_H_

Boolean lbTCPConnect( obj inst );
void  lbTCPDisconnect( obj inst );

int lbTCPRead ( obj inst, unsigned char *msg );
Boolean lbTCPWrite( obj inst, unsigned char *msg, int len );
Boolean lbTCPAvailable( obj inst );

#endif /* LBTCP_H_ */
