/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net




 All rights reserved.
*/

#ifndef ZIMOCAN_CONST_H_
#define ZIMOCAN_CONST_H_

#define PCID 0xC200

#define HEADER 0x5A32
#define TAIL   0x325A

#define MODE_REQ 0x00
#define MODE_CMD 0x01
#define MODE_EVT 0x03

#define SYSTEM_CONTROL_GROUP 0X00
#define SYSTEM_POWER 0X00

#define Zs100_PortStateCmd_None   0
#define Zs100_PortStateCmd_Run    1
#define Zs100_PortStateCmd_SSP0   2
#define Zs100_PortStateCmd_SSPE   3
#define Zs100_PortStateCmd_OFF    4
#define Zs100_PortStateCmd_SProg  5

#define SYSTEM_POWER_ON 0X00
#define SYSTEM_POWER_OFF 0X01
#define SYSTEM_POWER_TRACK1 0X00
#define SYSTEM_POWER_TRACK2 0X01
#define SYSTEM_POWER_TRACK_ALL 0XFF

#define ACCESSORY_COMMAND_GROUP 0X01
#define ACCESSORY_PORT 0X04
#define ACCESSORY_DATA 0X05

#define MOBILE_CONTROL_GROUP 0X02
#define MOBILE_STATE 0X00
#define MOBILE_MODE 0X01
#define MOBILE_SPEED 0X02
#define MOBILE_FUNCTION 0X04

#define TRACK_CONFIG_GROUP 0X06
#define TRACK_CONFIG_INFO 0X01
#define TRACK_CONFIG_READ 0X0A
#define TRACK_CONFIG_WRITE 0X0B

#define DATA_GROUP 0X07
#define DATA_NAME 0X21

#define NETWORK_GROUP 0X0A
#define NETWORK_PING  0X00
#define NETWORK_MODULINFO  0X08

#endif
