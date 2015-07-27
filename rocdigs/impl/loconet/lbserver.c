/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 All rights reserved.
*/

#include "rocdigs/impl/loconet_impl.h"

#include "rocdigs/impl/loconet/lbserver.h"

#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"

#include "rocrail/wrapper/public/DigInt.h"

static const char* RECEIVE_PREFIX = "RECEIVE";
static const char* SEND_PREFIX = "SEND";


Boolean lbserverConnect( obj inst ) {
  iOLocoNetData data = Data(inst);

  TraceOp.trc( "lbserver", TRCLEVEL_INFO, __LINE__, 9999, "LocoNet lbserver at %s:%d",
      wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ) );

  data->socket = SocketOp.inst( wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ), False, False, False );

  if ( SocketOp.connect( data->socket ) ) {
    return True;
  }
  else {
    SocketOp.base.del( data->socket );
    return False;
  }
}

void  lbserverDisconnect( obj inst ) {
  iOLocoNetData data = Data(inst);
  if( data->socket != NULL ) {
    SocketOp.disConnect( data->socket );
    SocketOp.base.del( data->socket );
    data->socket = NULL;
  }
}

Boolean lbserverAvailable ( obj inst ) {
  iOLocoNetData data = Data(inst);
  char msgStr[32];
  return SocketOp.peek( data->socket, msgStr, 1 );
}


int lbserverRead ( obj inst, unsigned char *msg ) {
  iOLocoNetData data = Data(inst);
  int i = 0;
  int msglen = 0;
  char msgStr[256];
  Boolean ok = True;
  
  if( !SocketOp.peek( data->socket, msgStr, 1 ) )
    return 0;
    
  if( SocketOp.readln( data->socket, msgStr ) ) {
  /* line may begin with 
      SENT message: last command was sent (or not)
      RECEIVE message: new message from loconet
      VERSION text: VERSION information about the server */
    char* receive = StrOp.find( msgStr, "RECEIVE");
  
    TraceOp.trc( "lbserver", TRCLEVEL_DEBUG, __LINE__, 9999, "msgStr=[%s]", msgStr );
    
    if( receive != NULL ) {
      char *d;
      int opCode = 0;
      int byte2  = 0;
      iOStrTok tok = StrTokOp.inst( receive, ' ' );
      const char* leadinStr = NULL;
      const char* byteStr = NULL;

      TraceOp.trc( "lbserver", TRCLEVEL_DEBUG, __LINE__, 9999, "receive=[%s]", receive );
      
      if( ok && StrTokOp.hasMoreTokens(tok) ) {
        leadinStr = StrTokOp.nextToken( tok );
        TraceOp.trc( "lbserver", TRCLEVEL_DEBUG, __LINE__, 9999, "leadinStr [%s]", leadinStr );
      }
      
      if( ok && (ok= StrTokOp.hasMoreTokens(tok)) ) {
        byteStr = StrTokOp.nextToken( tok );
        opCode  = strtol( byteStr, &d, 16 );
        TraceOp.trc( "lbserver", TRCLEVEL_DEBUG, __LINE__, 9999, "opCode %d [%s]", opCode, byteStr );
      }
      
      if( ok && (ok= StrTokOp.hasMoreTokens(tok)) ) {
        byteStr = StrTokOp.nextToken( tok );
        byte2   = strtol( byteStr, &d, 16 );
        TraceOp.trc( "lbserver", TRCLEVEL_DEBUG, __LINE__, 9999, "byte2 %d [%s]", byte2, byteStr );
      }
      
      msg[0] = (byte)opCode;
      msg[1] = (byte)byte2;
      
      switch((opCode & 0x60) >> 5) {
        case 0:     /* 2 byte message */
          msglen = 2;
          break;

        case 1:     /* 4 byte message */
          msglen = 4;
          break;
 
        case 2:     /* 6 byte message */
          msglen = 6;
          break;

        case 3:     /* N byte message */
          if (byte2<2) 
            TraceOp.trc( "lbserver", TRCLEVEL_MONITOR, __LINE__, 9999, "LocoNet message length invalid: 0x%0X opcode=0x%0X", byte2, opCode );
          msglen = byte2;
          break;
      }
      
      for( i = 2; ok && i < msglen && StrTokOp.hasMoreTokens( tok ); i++)  {
        byteStr = StrTokOp.nextToken( tok );
        msg[i] = strtol( byteStr, &d, 16 );
      }
      
      StrTokOp.base.del(tok);

      TraceOp.dump ( "lbserver", TRCLEVEL_BYTE, (char*)msg, msglen );
      
      
    }
  }
  return ok?msglen:0;
}


Boolean lbserverWrite( obj inst, unsigned char *msg, int len ) {
  iOLocoNetData data = Data(inst);
  unsigned char i;
  char *msgStr = NULL;
  char tmp[10];
  Boolean ok = False;

  msgStr = StrOp.cat(msgStr, "SEND");
  for (i = 0; i < len; i++) {
    StrOp.fmtb(tmp, " %02X", msg[i]);
    msgStr = StrOp.cat(msgStr, tmp);
  }
  msgStr = StrOp.cat(msgStr, "\r\n");
  ok = SocketOp.write( data->socket, msgStr, StrOp.len(msgStr) );
  StrOp.free(msgStr);
  return ok;
}
