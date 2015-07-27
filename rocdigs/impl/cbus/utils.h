/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/


#ifndef CBUS_UTILS_H_
#define CBUS_UTILS_H_
/*
 * CBUS Priorities
 */
#define PRIORITY_HIGH 0
#define PRIORITY_ABOVE 1
#define PRIORITY_NORMAL 2
#define PRIORITY_LOW 3

#define OFFSET_SIDH 2
#define OFFSET_SIDL 4
#define OFFSET_TYPE 6
#define OFFSET_OPC 7
#define OFFSET_D1 9
#define OFFSET_D2 11
#define OFFSET_D3 13
#define OFFSET_D4 15
#define OFFSET_D5 17
#define OFFSET_D6 19
#define OFFSET_D7 21

int makeFrame(byte* frame, int prio, byte* cmd, int datalen, int cid, Boolean eth );
int makeExtFrame(byte* frame, int prio, byte* cmd, int datalen, int cid );
byte HEXA2Byte( const byte* s );
void Byte2HEXA( byte* s, byte b );

#endif /* CBUS_UTILS_H_ */
