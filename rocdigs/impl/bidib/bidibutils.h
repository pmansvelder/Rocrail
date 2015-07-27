/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef BIDIBUTILS_H_
#define BIDIBUTILS_H_

/* C++ */
#ifdef __cplusplus
  extern "C" {
#endif

char* bidibGetClassName(int classid, char* mnemonic, Boolean* bridge );
const char* bidibGetFeatureName(int feature);
int bidibDeEscapeMessage(byte* msg, int inLen);
void bidibUpdateCRC(byte newb, byte* crc);
byte bidibCheckSum(byte* packet, int len);
void bidibEscapeMessage(byte* msg, int* newLen, int inLen);
int bidibMakeMessage(byte* msg, int inLen);
const char* bidibGetAccError(int rc);
const char* bidibGetPTState(int state);
const char* bidibGetDynStateName(int dynnum);

/* C++ */
#ifdef __cplusplus
  }
#endif

#endif /* BIDIBUTILS_H_ */
