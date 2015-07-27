/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifndef SRCPSERIAL_H_
#define SRCPSERIAL_H_

Boolean serialInit( obj inst );
int serialConnect( obj inst, Boolean info );
void  serialDisconnect( obj inst, Boolean info );

int serialRead ( obj inst, char *cmd, Boolean info );
int serialWrite( obj inst, const char *cmd, char* rsp, Boolean info );
Boolean serialAvailable( obj inst );

#endif /*SRCPSERIAL_H_*/
