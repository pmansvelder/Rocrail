/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 All rights reserved.
*/
#include "rocdigs/impl/zimocan_impl.h"
#include "rocdigs/impl/zimocan/serial.h"
#include "rocrail/wrapper/public/DigInt.h"

#define BAUDRATE 512000

Boolean SerialConnect( obj inst ) {
  iOZimoCANData data = Data(inst);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device  = %s", wDigInt.getdevice( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bps     = %d", BAUDRATE );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "timeout = %d", wDigInt.gettimeout( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( wDigInt.getdevice( data->ini ) );
  SerialOp.setFlow( data->serial, cts );
  SerialOp.setLine( data->serial, BAUDRATE, 8, 1, none, wDigInt.isrtsdisabled( data->ini ) );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( data->ini ), wDigInt.gettimeout( data->ini ) );

  if( SerialOp.open( data->serial ) ) {
    return True;
  }
  else {
    SerialOp.base.del( data->serial );
    data->serial = NULL;
    return False;
  }
}


void SerialDisconnect( obj inst ) {
  iOZimoCANData data = Data(inst);
  if( data->serial != NULL ) {
    SerialOp.close( data->serial );
    SerialOp.base.del( data->serial );
    data->serial = NULL;
  }
}


int SerialRead ( obj inst, unsigned char *msg ) {
  iOZimoCANData data = Data(inst);
  int len = 0;
  int packetLen = 11;
  int dlc = 0;
  Boolean headerZ = False;
  Boolean header2 = False;

  if( data->serial != NULL ) {
    do {
      char in = 0;
      if( SerialOp.read(data->serial, &in, 1) ) {
        if( !headerZ && in == 'Z' ) {
          headerZ = True;
          msg[len] = in;
          len++;
          continue;
        }
        if( headerZ && !header2 && in == '2' ) {
          header2 = True;
          msg[len] = in;
          len++;
          continue;
        }

        if( headerZ && header2 && len < (packetLen+dlc) ) {
          if( len == 2 )
            dlc = in;
          msg[len] = in;
          len++;
        }

        if( len == (packetLen+dlc) ) {
          TraceOp.dump ( "zcanserial", TRCLEVEL_BYTE, (char*)msg, len );
          break;
        }

      }
      else {
        break;
      }
    } while(data->serial != NULL);
  }
  return len;
}


Boolean SerialWrite( obj inst, unsigned char *msg, int len ) {
  iOZimoCANData data = Data(inst);
  Boolean ok = False;
  if( data->serial != NULL ) {
    TraceOp.dump ( "zcanserial", TRCLEVEL_BYTE, (char*)msg, len );
    ok = SerialOp.write( data->serial, (char*)msg, len );
  }
  return ok;
}


Boolean SerialAvailable( obj inst ) {
  iOZimoCANData data = Data(inst);
  if( data->serial != NULL )
    return SerialOp.available(data->serial);
  return 0;
}

