 /*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef ATLAS_H_
#define ATLAS_H_
/*
 * Atlas specific handling.
 */
Boolean atlasConnect(obj xpressnet);
void atlasDisConnect(obj xpressnet);
Boolean atlasAvail(obj xpressnet);
void atlasInit(obj xpressnet);
int atlasRead(obj xpressnet, byte* buffer, Boolean* rspreceived);
Boolean atlasWrite(obj xpressnet, byte* buffer, Boolean* rspexpected);


#endif /* ATLAS_H_ */
