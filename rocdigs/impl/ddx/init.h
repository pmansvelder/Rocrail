/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 

 Some parts are copied from the DDL project of Torsten Vogt: http://www.vogt-it.com/OpenSource/DDL

 All rights reserved.
*/
#ifndef __IMPL_DDX_INIT_H__
#define __IMPL_DDX_INIT_H__ 

#include "rocrail/wrapper/public/DDX.h"
#include "rocs/public/node.h"
#include "rocs/public/serial.h"

//void rocrail_ddxFbListener( obj inst, int addr, int state );
//void rocrail_ddxStateChanged( obj inst);

int ddx_entry(obj inst, iONode ddx_ini);
Boolean init_serialport(obj inst); 
void close_comport(obj inst);
void start_voltage(obj inst);
void stop_voltage(obj inst);
void quit(obj inst);

#endif
