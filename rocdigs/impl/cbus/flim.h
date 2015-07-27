/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef CBUSFLIM_H_
#define CBUSFLIM_H_

iONode processFLiM(obj inst, int opc, byte *frame, byte **extraMsg);
byte* programFLiM(obj inst, iONode node);

#endif /* CBUSFLIM_H_ */
