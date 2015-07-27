 /*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef RNUTILS_H_
#define RNUTILS_H_

unsigned char rnChecksum(const unsigned char *b, int len);

int rnCheckPacket(unsigned char* rn, int* extended, int* event);

int rnSenderAddrFromPacket(unsigned char* rn, int seven);
int rnReceipientAddrFromPacket(unsigned char* rn, int seven);
void rnSenderAddresToPacket( int addr, unsigned char* rn, int seven );
void rnReceipientAddresToPacket( int addr, unsigned char* rn, int seven );
const char* rnActionTypeString(unsigned char* rn);
int rnActionFromPacket(unsigned char* rn);
int rnActionTypeFromPacket(unsigned char* rn);
const char* rnClassString(int pclass, char* mnemonic);
const char* rnGetRC(int rc);
const char* rnGetRS(int rs);



#endif /*RNUTILS_H_*/
