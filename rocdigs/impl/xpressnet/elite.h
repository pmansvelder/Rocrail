 /*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef ELITE_H_
#define ELITE_H_
/*
 * Hornby Elite specific handling.
 */
Boolean eliteConnect(obj xpressnet);
void eliteDisConnect(obj xpressnet);
Boolean eliteAvail(obj xpressnet);
void eliteInit(obj xpressnet);
int eliteRead(obj xpressnet, byte* buffer, Boolean* rspreceived);
Boolean eliteWrite(obj xpressnet, byte* buffer, Boolean* rspexpected);


#endif /* ELITE_H_ */
