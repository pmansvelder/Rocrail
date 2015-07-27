/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef BIDIBUDP_H_
#define BIDIBUDP_H_

Boolean udpInit( obj inst );
Boolean udpConnect( obj inst );
void  udpDisconnect( obj inst );

int udpRead ( obj inst, unsigned char *msg );
Boolean udpWrite( obj inst, unsigned char *path, unsigned char code, unsigned char* data, int datalen, void* node );
Boolean udpAvailable( obj inst );

#endif /* BIDIBUDP_H_ */
