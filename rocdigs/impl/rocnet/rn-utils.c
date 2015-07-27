 /*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 All rights reserved.
*/



#include "rocdigs/impl/rocnet/rn-utils.h"
#include "rocdigs/impl/rocnet/rocnet-const.h"
#include "rocs/public/trace.h"

static const char* name = "rnutils";


unsigned char rnChecksum(const unsigned char *b, int len) {
  unsigned char chksum = 0xff;
  int i;
  for (i = 0; i < len; i++) {
    chksum ^= b[i];
  }
  return chksum;
}


int rnCheckPacket(unsigned char* rn, int* extended, int* event) {
  return True;
}


int rnActionFromPacket(unsigned char* rn) {
  return rn[RN_PACKET_ACTION] & RN_ACTION_CODE_MASK;
}
int rnActionTypeFromPacket(unsigned char* rn) {
  return (rn[RN_PACKET_ACTION] >> 5 ) & 3;
}


int rnSenderAddrFromPacket(unsigned char* rn, int seven) {
  return rn[RN_PACKET_SNDRL] + rn[RN_PACKET_SNDRH] * (seven?128:256);
}

int rnReceipientAddrFromPacket(unsigned char* rn, int seven) {
  return rn[RN_PACKET_RCPTL] + rn[RN_PACKET_RCPTH] * (seven?128:256);
}

void rnSenderAddresToPacket( int addr, unsigned char* rn, int seven ) {
  rn[RN_PACKET_SNDRL] = addr % (seven?128:256);
  rn[RN_PACKET_SNDRH] = addr / (seven?128:256);
}

void rnReceipientAddresToPacket( int addr, unsigned char* rn, int seven ) {
  rn[RN_PACKET_RCPTL] = addr % (seven?128:256);
  rn[RN_PACKET_RCPTH] = addr / (seven?128:256);
}

const char* rnActionTypeString(unsigned char* rn) {
  int actionType = (rn[RN_PACKET_ACTION] >> 5 ) & 3;
  switch(actionType) {
    case RN_ACTIONTYPE_REQUEST:
      return "request";
    case RN_ACTIONTYPE_EVENT:
      return "event";
    case RN_ACTIONTYPE_REPLY:
      return "reply";
    default:
      return "unsupported";
  }
}

const char* rnClassString(int classid, char* mnemonic) {
  char* classname = NULL;
  int idx = 0;

  mnemonic[idx] = '\0';
  if( classid & RN_CLASS_IO ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, "Accessory");
    mnemonic[idx] = 'A';
    idx++;
    mnemonic[idx] = '\0';
  }
  if( classid & RN_CLASS_DCC ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, "DCC");
    mnemonic[idx] = 'D';
    idx++;
    mnemonic[idx] = '\0';
  }
  if( classid & RN_CLASS_RFID ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, "RFID");
    mnemonic[idx] = 'I';
    idx++;
    mnemonic[idx] = '\0';
  }

  return classname;
}

const char* rnGetRC(int rc) {
  switch( rc ) {
  case RN_ERROR_RC_IO:
    return "GPIO";
  case RN_ERROR_RC_I2C:
    return "I2C";
  case RN_ERROR_RC_RFID:
    return "RFID";
  }
  return "unknown";
}

const char* rnGetRS(int rs) {
  switch( rs ) {
  case RN_ERROR_RS_SETUP:
    return "setup";
  case RN_ERROR_RS_WRITE:
    return "write";
  case RN_ERROR_RS_READ:
    return "read";
  case RN_ERROR_RS_ACK:
    return "acknowledge";
  }
  return "unknown";
}



