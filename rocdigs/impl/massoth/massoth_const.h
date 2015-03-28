#define LC_SPEEDDATA     0x61
#define LC_FUNCTIONDATA  0x62
#define LC_REGISTER      0x64
#define LC_CONFIGURE     0x85
#define LC_DELETE        0x45
#define LC_ADDRESS_READ  0x89
#define LC_SAVE          0x1D

#define DCC_NMRA_MASK    0x08

#define SW_COMMAND       0x4A

#define FB_EVENT         0x4B

#define EBREAK_RESET        0x10
#define EBREAK_SET          0x11
#define EBREAK_RESET_PACKET 0x12

#define AUTO_FUNCTION    0xD3

#define PROG_REGISTER    0x54
#define PROG_CV          0x75
#define PROG_POM         0xB5
#define PROG_READ_CV     0x56

#define INTERFACE_REGISTER  0xB8


#define CS_SYSTEM        0x00
#define CS_SYSTEM_DATA1  0x01

#define CS_UPDATE        0x00
#define CS_UPDATE_DATA1  0x03

#define CS_LOAD          0x00
#define CS_LOAD_DATA1    0x05

#define CS_LC_FREE       0x40
#define CS_LC_FREE_DATA1 0x08

#define CS_LC_INVALID       0x40
#define CS_LC_INVALID_DATA1 0x04

#define CS_LC_LOGOUT       0x60
#define CS_LC_LOGOUT_DATA1 0x03

#define CS_PROG_ACK        0x80
#define CS_PROG_ACK_DATA1  0x02

#define CS_PROG_CV        0x80
#define CS_PROG_CV_DATA1  0x04

#define CS_ERROR_XOR       0x20
#define CS_ERROR_XOR_DATA1 0x01
#define CS_ERROR_XOR_DATA2 0xFF
