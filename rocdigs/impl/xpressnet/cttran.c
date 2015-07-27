/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 All rights reserved.
*/

#include "rocdigs/impl/xpressnet_impl.h"
#include "rocdigs/impl/xpressnet/cttran.h"
#include "rocdigs/impl/xpressnet/li101.h"

Boolean cttranConnect(obj xpressnet) {
  return li101Connect(xpressnet);
}

void cttranDisConnect(obj xpressnet) {
  li101DisConnect(xpressnet);
}

Boolean cttranAvail(obj xpressnet) {
  return li101Avail(xpressnet);
}

void cttranInit(obj xpressnet) {
  li101Init(xpressnet);
}
int cttranRead(obj xpressnet, byte* buffer, Boolean* rspreceived) {
  return li101Read(xpressnet, buffer, rspreceived);
}
Boolean cttranWrite(obj xpressnet, byte* buffer, Boolean* rspexpected) {
  return li101Write(xpressnet, buffer, rspexpected);
}
