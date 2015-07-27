 /*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/


#ifndef OPENDCC_H_
#define OPENDCC_H_
/*
 * OpenDCC specific handling.
 */
Boolean opendccConnect(obj xpressnet);
void opendccDisConnect(obj xpressnet);
Boolean opendccAvail(obj xpressnet);
void opendccInit(obj xpressnet);
int opendccRead(obj xpressnet, byte* buffer, Boolean* rspreceived);
Boolean opendccWrite(obj xpressnet, byte* buffer, Boolean* rspexpected);
void opendccTranslate(obj xpressnet, void* node);



#endif /* OPENDCC_H_ */
