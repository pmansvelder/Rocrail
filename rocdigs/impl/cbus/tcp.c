/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 All rights reserved.
*/

#include "rocdigs/impl/cbus_impl.h"

#include "rocdigs/impl/cbus/tcp.h"
#include "rocdigs/impl/cbus/cbusdefs.h"
#include "rocdigs/impl/cbus/utils.h"

#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/thread.h"
#include "rocs/public/serial.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"

Boolean tcpConnectInternal( obj inst );

static void __watchdog( void* threadinst ) {
  iOThread     th = (iOThread)threadinst;
  iOCBUS     cbus = (iOCBUS)ThreadOp.getParm(th);
  iOCBUSData data = Data(cbus);

  int retry = 0;
  int stick = 0;

  ThreadOp.sleep(10);
  TraceOp.trc( "cbustcp", TRCLEVEL_INFO, __LINE__, 9999, "cbus tcp watchdog started" );

  while( data->run ) {
    if( data->socket == NULL ) {
      TraceOp.trc( "cbustcp", TRCLEVEL_DEBUG, __LINE__, 9999, "connecting..." );
      if( !data->connectpending && retry < 60 && !tcpConnectInternal((obj)cbus) ) {
        retry++;
      }
      else {
        retry = 0;
      }
    }
    else if( SystemOp.getTick() - data->lastcmdtick > 100 ){
      byte cmd[2];
      byte* frame = allocMem(32);
      cmd[0] = 0;
      makeFrame(frame, PRIORITY_NORMAL, cmd, 0, data->cid, True );
      ThreadOp.post(data->writer, (obj)frame);
      TraceOp.trc( "cbustcp", TRCLEVEL_DEBUG, __LINE__, 9999, "keep alive message" );
      if ( stick++ > 60 ) {
        byte cmd[2];
        byte* frame = allocMem(32);
        cmd[0] = 3;
        makeFrame(frame, PRIORITY_NORMAL, cmd, 0, data->cid, True );
        ThreadOp.post(data->writer, (obj)frame);
        stick = 0;
      }
    }
    ThreadOp.sleep(1000);
  }
  TraceOp.trc( "cbustcp", TRCLEVEL_INFO, __LINE__, 9999, "cbus tcp watchdog ended" );
}


Boolean tcpConnect( obj inst ) {
  iOCBUSData data = Data(inst);

  if( !data->enablecom ) {
    return False;
  }

  data->lastcmdtick = SystemOp.getTick();
  data->connectpending = False;
  data->socket = NULL;
  if( data->watchdog == NULL ) {
    data->watchdog = ThreadOp.inst( "cbustcpwd", &__watchdog, inst );
    ThreadOp.start( data->watchdog );
  }
  return True;
}

Boolean tcpConnectInternal( obj inst ) {
  iOCBUSData data = Data(inst);
  iOSocket socket = NULL;

  if( !data->enablecom ) {
    return False;
  }

  data->connectpending = True;

  if( wDigInt.gethost( data->ini ) == NULL || StrOp.len(wDigInt.gethost( data->ini )) == 0 ) {
    wDigInt.sethost( data->ini, "192.168.0.200" );
  }
  if( wDigInt.getport( data->ini ) == 0 ) {
    wDigInt.setport( data->ini, 5550 );
  }

  TraceOp.trc( "cbustcp", TRCLEVEL_WARNING, __LINE__, 9999,
      "trying to connect to %s:%d...", wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ) );

  socket = SocketOp.inst( wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ), False, False, False );
  SocketOp.setRcvTimeout( socket, wDigInt.gettimeout(data->ini) / 1000);
  SocketOp.setNodelay(socket, True);

  if ( SocketOp.connect( socket ) ) {
    data->socket = socket;
    TraceOp.trc( "cbustcp", TRCLEVEL_WARNING, __LINE__, 9999, "connected to %s:%d", wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ) );
    data->connectpending = False;
    return True;
  }
  else {
    TraceOp.trc( "cbustcp", TRCLEVEL_WARNING, __LINE__, 9999, "unable to connect to %s:%d; check the network...",
        wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ) );
    if( socket != NULL ) {
      SocketOp.base.del( socket );
      socket = NULL;
    }
    data->connectpending = False;
    return False;
  }

  return False;
}


void tcpDisconnect( obj inst ) {
  iOCBUSData data = Data(inst);

  if( data->socket != NULL ) {
    TraceOp.trc( "cbustcp", TRCLEVEL_INFO, __LINE__, 9999, "disconnecting..." );
    iOSocket socket = data->socket;
    data->socket = NULL;
    SocketOp.disConnect( socket );
    SocketOp.base.del( socket );
  }
}



Boolean tcpRead ( obj inst, unsigned char *frame, int len ) {
  iOCBUSData data = Data(inst);

  if( !data->enablecom ) {
    return True;
  }

  if( data->socket == NULL ) {
    return False;
  }
  
  if( !SocketOp.isBroken(data->socket) && SocketOp.read( data->socket, (char*)frame, len ) ) {
    return True;
  }
  else {
    TraceOp.trc( "cbustcp", TRCLEVEL_EXCEPTION, __LINE__, 9999, "problem reading CBUS: Disconnect" );
    tcpDisconnect(inst);
    ThreadOp.sleep(1000);
  }
  return False;
}


Boolean tcpWrite( obj inst, unsigned char *frame, int len ) {
  iOCBUSData data = Data(inst);

  if( !data->enablecom ) {
    return True;
  }

  if( data->socket == NULL ) {
    return False;
  }

  if( !SocketOp.isBroken(data->socket) && SocketOp.write( data->socket, (char*)frame, len ) ) {
    TraceOp.dump ( "cbustcp", TRCLEVEL_BYTE, (char*)frame, len );
    data->lastcmdtick = SystemOp.getTick();
    return True;
  }
  else {
    TraceOp.trc( "cbustcp", TRCLEVEL_WARNING, __LINE__, 9999, "could not write to CBUS" );
    tcpDisconnect(inst);
    ThreadOp.sleep(1000);
  }

  return False;
}


Boolean tcpAvailable( obj inst ) {
  iOCBUSData data = Data(inst);
  char msgStr[32];

  if( !data->enablecom ) {
    return False;
  }

  if( data->socket == NULL || SocketOp.isBroken(data->socket) ) {
    TraceOp.trc( "cbustcp", TRCLEVEL_DEBUG, __LINE__, 9999, "not connected" );
    return False;
  }
  return SocketOp.peek( data->socket, msgStr, 1 );
}

