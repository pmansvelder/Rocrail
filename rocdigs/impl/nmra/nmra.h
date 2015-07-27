/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 

 Some parts are copied from the DDL project of Torsten Vogt: http://www.vogt-it.com/OpenSource/DDL

 All rights reserved.
*/
#ifndef __IMPL_NMRA_H__
#define __IMPL_NMRA_H__

#include "rocs/public/mem.h"


/* signal generating functions for nmra dcc */

int idlePacket(char* packetstream, Boolean longIdle);

/* standard decoder */
int compSpeed14(char* packetstream, int address, int direction, int speed);


int compSpeed(char* packetstream, int address, Boolean longaddr, int direction, int speed, int steps);
int compFunction(char* packetstream, int address, Boolean longaddr, int group, Boolean f[]);
int compBinStat(char* packetstream, int address, Boolean longaddr, int nr, int val);

/* short addresses */
int compSpeed28ShortAddr(char* packetstream, int address, int direction, int speed);
int compSpeed128ShortAddr(char* packetstream, int address, int direction, int speed);
int compFunctionShortAddr(char* packetstream, int address, int group, Boolean f[]);

/* long addresses */
int compSpeed28LongAddr(char* packetstream, int address, int direction, int speed);
int compSpeed128LongAddr(char* packetstream, int address, int direction, int speed);
int compFunctionLongAddr(char* packetstream, int address, int group, Boolean f[]);

/* accessory decoder */
int compAccessory(char* packetstream, int addr, int port, int gate, int activate);

/* POM */
int dccPOM(char* packetstream, int address, Boolean longaddr, int cvNum, int data, Boolean verify);

/* PT */
int createCVgetpacket(int cv, int value, char* SendStream, int start);
int createCVsetpacket(int cv, int value, char* SendStream, int verify);
char* getResetStream(int *rsSize);


#endif
