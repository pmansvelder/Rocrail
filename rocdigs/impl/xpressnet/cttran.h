 /*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef CTTRAN_H_
#define CTTRAN_H_
/*
 * cttran specific handling.
 */
Boolean cttranConnect(obj xpressnet);
void cttranDisConnect(obj xpressnet);
Boolean cttranAvail(obj xpressnet);
void cttranInit(obj xpressnet);
int cttranRead(obj xpressnet, byte* buffer, Boolean* rspreceived);
Boolean cttranWrite(obj xpressnet, byte* buffer, Boolean* rspexpected);


#endif /* CTTRAN_H_ */
