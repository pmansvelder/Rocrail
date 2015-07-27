/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef ROCNETNODE_IO_H_
#define ROCNETNODE_IO_H_

#define IO_I2C_1 0
#define IO_I2C_0 1
#define IO_I2C_2 2
#define IO_DIRECT 3

#define IO_OUTPUT 0
#define IO_INPUT 1

#define IO_TYPE   0x0F
#define IO_BLINK  0x80
#define IO_INVERT 0x40
#define IO_TOGGLE 0x20

#define IO_LED1 23
#define IO_LED2 24
#define IO_PB1 25
#define IO_RELAIS 9

#define PWM_BLINK  0x80
#define PWM_REPORT 0x40
#define PWM_SERVO  0x20


void raspiGPIOAlt(int g, int alt);
int raspiSetupIO(void);
void raspiConfigPort(int port, int type);
int raspiRead(int port);
void raspiWrite(int port, int val);
int raspiDummy(void);
void raspiSetPWM(int percent);
void raspiInitPWM(int maxpwm);
#endif

