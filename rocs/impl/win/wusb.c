/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 All rights reserved.
*/
#include "rocs/impl/usb_impl.h"

#include "rocs/public/mem.h"


void* rocs_usb_openUSB(int vendor, int product, int configNr, int interfaceNr, int* input, int* output) {
  return NULL;
}

Boolean rocs_usb_closeUSB(void* husb, int interfaceNr) {
  return False;
}

int rocs_usb_writeUSB(void* husb, int endpoint, byte* out, int len, int timeout) {
  return 0;
}

int rocs_usb_readUSB(void* husb, int endpoint, byte* in, int len, int timeout) {
  return 0;
}
