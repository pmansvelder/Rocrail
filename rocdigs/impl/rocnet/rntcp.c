/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 All rights reserved.
*/
#include "rocdigs/impl/rocnet_impl.h"
#include "rocdigs/impl/rocnet/rntcp.h"

#include "rocrail/wrapper/public/RocNet.h"

Boolean rnTcpConnect( obj inst ) {
  iOrocNetData data = Data(inst);
  return True;
}

void rnTcpDisconnect( obj inst ) {
  iOrocNetData data = Data(inst);
}


int rnTcpRead ( obj inst, unsigned char *msg ) {
  iOrocNetData data = Data(inst);
  return 0;
}

Boolean rnTcpWrite( obj inst, unsigned char *msg, int len ) {
  iOrocNetData data = Data(inst);
  return True;
}


Boolean rnTcpAvailable( obj inst ) {
  iOrocNetData data = Data(inst);
  return True;
}

