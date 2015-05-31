/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

 


 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include "rocrail/public/clntcon.h"

#include "rocrail/impl/pclient_impl.h"
#include "rocrail/impl/web/rocweb.h"


#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/trace.h"


static int instCnt = 0;

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iOPClientData data = Data(inst);
    /* Cleanup data->xxx members...*/
    data->websocketrun = False;
    if( data->socket != NULL && data->websocket ) {
      rocWebSocketClose(inst);
      SocketOp.base.del(data->socket);
      data->socket = NULL;
    }
    ThreadOp.sleep(50);
    freeMem( data );
    freeMem( inst );
    instCnt--;
  }
  return;
}

static const char* __name( void ) {
  return name;
}

static unsigned char* __serialize( void* inst, long* size ) {
  return NULL;
}

static void __deserialize( void* inst,unsigned char* bytestream ) {
  return;
}

static char* __toString( void* inst ) {
  return NULL;
}

static int __count( void ) {
  return instCnt;
}

static struct OBase* __clone( void* inst ) {
  return NULL;
}

static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}

static void* __properties( void* inst ) {
  return NULL;
}

/** ----- OPClient ----- */




/** PClient ID. */
static const char* _getId( struct OPClient* inst ) {
  iOPClientData data = Data(inst);
  return data->cid;
}


/** Object creator. */
static struct OPClient* _inst( iOSocket socket ,iONode ini ) {
  iOPClient __PClient = allocMem( sizeof( struct OPClient ) );
  iOPClientData data = allocMem( sizeof( struct OPClientData ) );
  MemOp.basecpy( __PClient, &PClientOp, 0, sizeof( struct OPClient ), data );

  /* Initialize data->xxx members... */
  data->ini           = ini;
  data->socket        = socket;
  data->cid           = StrOp.fmt( "%08X", __PClient );

  instCnt++;
  return __PClient;
}




/** Work slice. */
static Boolean _work( struct OPClient* inst, iONode event, char** command ) {
  if( inst != NULL ) {
    iOPClientData data = Data(inst);
    char str[1025] = {'\0'};

    if( data->socket == NULL )
      return True;
    if( !SocketOp.setRcvTimeout( data->socket, 10 ) )
      return True;
    if( !SocketOp.setSndTimeout( data->socket, 10 ) )
      return True;

    if( data->websocket ) {
      char* cmd = NULL;
      Boolean ok = rocWebSocket( inst, event, &cmd );
      *command = cmd;
      if( cmd != NULL ) {
        /* Parse command and send it over the callback function to the control. */
        TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "command received: %.120s", cmd );
      }
      return ok;
    }


    /* Read first HTTP header line: */
    TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "Read first HTTP header line..." );
    if( !SocketOp.readln( data->socket, str ) ) {
      SocketOp.disConnect( data->socket );
      return True;
    }
    TraceOp.dump( name, TRCLEVEL_USER2, str, StrOp.len(str) );


    return rocWeb( inst, str );
  }
  return True;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/pclient.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
