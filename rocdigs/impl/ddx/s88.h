/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 

 Some parts are copied from the DDL project of Torsten Vogt: http://www.vogt-it.com/OpenSource/DDL

 All rights reserved.
*/
#ifndef __IMPL_DDX_S88_H__
#define __IMPL_DDX_S88_H__

/*maximal number of bytes read from one s88-bus*/
#define S88_MAXPORTSB 62 
/*maximal number of s88-busses*/
#define S88_MAXBUSSES 4
/*maximal number of ports*/
#define S88_MAXPORTS S88_MAXPORTSB*8*S88_MAXBUSSES

int  s88init(obj inst); /* test and open the S88PORT and initalizes the bus*/
void start_polling_s88(obj inst);
void setPT(obj inst, int state);
int  isPT(obj inst);

typedef struct _tDelayedAccCmd {
   char acc_p;
   int  acc_addr;
   int  acc_port;
   int  acc_gate;
   int  action;
   int  delay;
   int  busnr;
   int  devicegroup;
} tDelayedAccCmd;

#endif

