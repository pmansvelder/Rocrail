 /*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef LIETH_H_
#define LIETH_H_
Boolean liethConnect(obj xpressnet);
void liethDisConnect(obj xpressnet);
Boolean liethAvail(obj xpressnet);
void liethInit(obj xpressnet);
int liethRead(obj xpressnet, byte* buffer, Boolean* rspreceived);
Boolean liethWrite(obj xpressnet, byte* buffer, Boolean* rspexpected);


#endif /* LIETH_H_ */
