/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 All rights reserved.
*/

#ifndef LNUTILS_H_
#define LNUTILS_H_

unsigned char checksumLN(const unsigned char* cmd, int len);
int binaryStateLN(unsigned char* msg, int addr, int nr, int val);


#endif /*LNUTILS_H_*/
