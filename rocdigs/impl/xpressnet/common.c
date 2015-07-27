 /*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 All rights reserved.
*/
#include "rocdigs/impl/xpressnet/common.h"
#include "rocs/public/trace.h"

Boolean isChecksumOK(byte* in) {
  byte bXor = 0;
  int i = 0;
  int datalen = (in[0] & 0x0f) + 1;

  for( i = 0; i < datalen; i++ ) {
    bXor ^= in[ i ];
  }

  if( bXor != in[datalen]) {
    TraceOp.trc( "xnxor", TRCLEVEL_EXCEPTION, __LINE__, 9999,
        "XOR error: datalength=%d calculated=0x%02X received=0x%02X", datalen, bXor, in[datalen] );
    return False;
  }
  return True;
}


int makeChecksum(byte* out) {
  int len = out[0] & 0x0f;
  int i = 0;
  byte bXor = 0;
  
  len++; /* header */

  if( out[0] == 0x00 ) {
    return 0;
  }

  for ( i = 0; i < len; i++ ) {
    bXor ^= out[i];
  }
  out[i] = bXor;
  len++; /* checksum */

  return len;
}



