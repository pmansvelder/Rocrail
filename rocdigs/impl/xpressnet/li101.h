 /*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/


#ifndef LI101_H_
#define LI101_H_
Boolean li101Connect(obj xpressnet);
void li101DisConnect(obj xpressnet);
Boolean li101Avail(obj xpressnet);
void li101Init(obj xpressnet);
int li101Read(obj xpressnet, byte* buffer, Boolean* rspreceived);
Boolean li101Write(obj xpressnet, byte* buffer, Boolean* rspexpected);


#endif /* LI101_H_ */
