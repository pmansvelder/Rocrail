/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 All rights reserved.
*/
#include "rocdigs/impl/rocnet_impl.h"
#include "rocdigs/impl/rocnet/rnudp.h"

#include "rocrail/wrapper/public/RocNet.h"


Boolean rnUDPConnect( obj inst ) {
  iOrocNetData data = Data(inst);

  data->readUDP = SocketOp.inst( wRocNet.getaddr(data->rnini), wRocNet.getport(data->rnini), False, True, True );
  SocketOp.bind(data->readUDP);
  data->writeUDP = SocketOp.inst( wRocNet.getaddr(data->rnini), wRocNet.getport(data->rnini), False, True, True );
  return True;
}


void rnUDPDisconnect( obj inst ) {
  iOrocNetData data = Data(inst);
}


int rnUDPRead ( obj inst, unsigned char *msg ) {
  iOrocNetData data = Data(inst);
  SocketOp.recvfrom( data->readUDP, (char*)msg, 0x7F, NULL, NULL );
  return 0;
}


Boolean rnUDPWrite( obj inst, unsigned char *msg, int len ) {
  iOrocNetData data = Data(inst);

  SocketOp.sendto( data->writeUDP, (char*)msg, len, NULL, 0 );

  return True;
}


Boolean rnUDPAvailable( obj inst ) {
  iOrocNetData data = Data(inst);
  return True;
}

