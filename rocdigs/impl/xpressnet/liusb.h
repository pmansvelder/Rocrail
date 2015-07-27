 /*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef LIUSB_H_
#define LIUSB_H_
Boolean liusbConnect(obj xpressnet);
void liusbDisConnect(obj xpressnet);
Boolean liusbAvail(obj xpressnet);
void liusbInit(obj xpressnet);
int liusbRead(obj xpressnet, byte* buffer, Boolean* rspreceived);
Boolean liusbWrite(obj xpressnet, byte* buffer, Boolean* rspexpected);


#endif /* LIUSB_H_ */
