 /*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef XNTCP_H_
#define XNTCP_H_
/*
 * XnTcp specific handling.
 */
Boolean xntcpConnect(obj xpressnet);
void xntcpDisConnect(obj xpressnet);
Boolean xntcpAvail(obj xpressnet);
void xntcpInit(obj xpressnet);
int xntcpRead(obj xpressnet, byte* buffer, Boolean* rspreceived);
Boolean xntcpWrite(obj xpressnet, byte* buffer, Boolean* rspexpected);


#endif /* XNTCP_H_ */
