/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifndef BIDIBSERIAL_H_
#define BIDIBSERIAL_H_

Boolean serialInit( obj inst );
Boolean serialConnect( obj inst );
void  serialDisconnect( obj inst );

int serialRead ( obj inst, unsigned char *msg );
Boolean serialWrite( obj inst, unsigned char *path, unsigned char code, unsigned char* data, int datalen, void* node );
Boolean serialAvailable( obj inst );

#endif /*BIDIBSERIAL_H_*/
