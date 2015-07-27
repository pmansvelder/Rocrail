/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 All rights reserved.
*/

#include "rocdigs/impl/xpressnet_impl.h"
#include "rocdigs/impl/xpressnet/atlas.h"
#include "rocdigs/impl/xpressnet/li101.h"

Boolean atlasConnect(obj xpressnet) {
  return li101Connect(xpressnet);
}

void atlasDisConnect(obj xpressnet) {
  li101DisConnect(xpressnet);
}

Boolean atlasAvail(obj xpressnet) {
  return li101Avail(xpressnet);
}

void atlasInit(obj xpressnet) {
  li101Init(xpressnet);
}
int atlasRead(obj xpressnet, byte* buffer, Boolean* rspreceived) {
  return li101Read(xpressnet, buffer, rspreceived);
}
Boolean atlasWrite(obj xpressnet, byte* buffer, Boolean* rspexpected) {
  return li101Write(xpressnet, buffer, rspexpected);
}
