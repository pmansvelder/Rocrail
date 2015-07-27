/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef ULNI_H_
#define ULNI_H_

Boolean ulniConnect( obj inst );
void  ulniDisconnect( obj inst );

int ulniRead ( obj inst, unsigned char *msg );
Boolean ulniWrite( obj inst, unsigned char *msg, int len );
Boolean ulniAvailable( obj inst );

#endif /* ULNI_H_ */
