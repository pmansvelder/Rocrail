/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/



#ifndef SRCPTCPIP_H_
#define SRCPTCPIP_H_


Boolean tcpipInit( obj inst );
int tcpipConnect( obj inst, Boolean info );
void tcpipDisconnect( obj inst, Boolean info );

int tcpipRead ( obj inst, char *cmd, Boolean info );
int tcpipWrite( obj inst, const char *cmd, char* rsp, Boolean info );
Boolean tcpipAvailable( obj inst );


#endif /* TCPIP_H_ */
