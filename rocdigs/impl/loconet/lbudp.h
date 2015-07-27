/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifndef LBUDP_H_
#define LBUDP_H_

Boolean lbUDPConnect( obj inst );
void  lbUDPDisconnect( obj inst );

int lbUDPRead ( obj inst, unsigned char *msg );
Boolean lbUDPWrite( obj inst, unsigned char *msg, int len );
Boolean lbUDPAvailable( obj inst );



#endif /* LBUDP_H_ */
