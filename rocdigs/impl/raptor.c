/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2015 Rob Versluis, Rocrail.net

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


#include "rocdigs/impl/raptor_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/system.h"
#include "rocs/public/strtok.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Command.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Feedback.h"

#include "rocutils/public/addr.h"

static int instCnt = 0;

static int vmajor = 2;
static int vminor = 0;
static int patch  = 99;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iORaptorData data = Data(inst);
    /* Cleanup data->xxx members...*/
    
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

static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/** ----- ORaptor ----- */

static Boolean __getFnState( iONode cmd, int fn ) {
  char fStr[32];
  StrOp.fmtb( fStr, "f%d", fn );
  return NodeOp.getBool( cmd, fStr, False );
}


static iONode __translate( iORaptor inst, iONode node ) {
  iORaptorData data = Data(inst);
  iONode rsp = NULL;
  char msg[256];

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cmd=%s", NodeOp.getName( node ) );

  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );
    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      char* cmd = StrOp.fmt("POW 0\n");
      ThreadOp.post( data->transactor, (obj)cmd );
    }
    else if( StrOp.equals( cmd, wSysCmd.ebreak ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "EBreak" );
      char* cmd = StrOp.fmt("DRI 0\n");
      ThreadOp.post( data->transactor, (obj)cmd );
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      char* cmd = StrOp.fmt("POW 1\n");
      ThreadOp.post( data->transactor, (obj)cmd );
      cmd = StrOp.fmt("DRI 1\n");
      ThreadOp.post( data->transactor, (obj)cmd );
    }
  }

  /* Loc command.*/
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int    addr = wLoc.getaddr( node );
    Boolean dir = wLoc.isdir( node );
    int       V = 0;

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        V = (wLoc.getV( node ) * 28) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        V = (wLoc.getV( node ) * 28) / wLoc.getV_max( node );
    }
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco %d %s%d", addr, dir?"":"-", V );
    char* cmd = StrOp.fmt("LOK %d %d %d\n", addr, V, dir?0:1);
    ThreadOp.post( data->transactor, (obj)cmd );
  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int addr = wFunCmd.getaddr(node);
    int fnchanged = wFunCmd.getfnchanged(node);
    char* cmd = StrOp.fmt("FNC %d %d %d\n", addr, fnchanged, __getFnState(node, fnchanged)?1:0);
    ThreadOp.post( data->transactor, (obj)cmd );
  }

  /* Switch command.
   */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int addr  = wSwitch.getaddr1( node );
    char state = StrOp.equals( wSwitch.getcmd( node ), wSwitch.straight ) ? 'G':'R';
    int port  = wSwitch.getport1( node );
    if( port > 0 ) {
      addr = AddrOp.toPADA( addr, port );
    }
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "switch %d %s", addr, wSwitch.getcmd(node) );
    char* cmd = StrOp.fmt("OUT %d %c\n", addr, state);
    ThreadOp.post( data->transactor, (obj)cmd );
  }



  return rsp;
}

/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iORaptorData data = Data(inst);
  iONode rsp = NULL;

  if( cmd != NULL ) {
    rsp = __translate((iORaptor)inst, cmd);
    cmd->base.del(cmd);
  }
  return rsp;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ,Boolean shutdown ) {
  iORaptorData data = Data(inst);
  data->run = False;
  ThreadOp.sleep(500);
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iORaptorData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


/**  */
static Boolean _setRawListener( obj inst ,obj listenerObj ,const digint_rawlistener listenerRawFun ) {
  return 0;
}


/** external shortcut event */
static void _shortcut( obj inst ) {
  return;
}


/** bit0=power, bit1=programming, bit2=connection */
static int _state( obj inst ) {
  return 0;
}


/**  */
static Boolean _supportPT( obj inst ) {
  return 0;
}


/** vmajor*1000 + vminor*100 + patch */
static int _version( obj inst ) {
  iORaptorData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static void __transactor( void* threadinst ) {
  iOThread        th = (iOThread)threadinst;
  iORaptor    raptor = (iORaptor)ThreadOp.getParm(th);
  iORaptorData data = Data(raptor);
  Boolean   serialOK = False;
  Boolean   didInit  = False;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Transactor started." );

  ThreadOp.sleep(100);

  while( data->run ) {

    if( !serialOK ) {
      data->serial = SerialOp.inst( wDigInt.getdevice( data->ini ) );
      SerialOp.setFlow( data->serial, 0 );
      SerialOp.setLine( data->serial, 9600, 8, 2, none, wDigInt.isrtsdisabled( data->ini ) );
      SerialOp.setTimeout( data->serial, wDigInt.gettimeout(data->ini), wDigInt.gettimeout(data->ini) );
      serialOK = SerialOp.open( data->serial );
      if( !serialOK ) {
        SerialOp.base.del(data->serial);
        data->serial = NULL;
        ThreadOp.sleep(2500);
        continue;
      }
    }

    if( !didInit ) {
      char* cmd = StrOp.fmt("VER\n");
      ThreadOp.post( data->transactor, (obj)cmd );
      didInit = True;
    }

    char* cmd = (char*)ThreadOp.getPost( th );
    if (cmd != NULL) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "write: %s", cmd );
      if( !SerialOp.write( data->serial, cmd, StrOp.len(cmd) ) ) {
        SerialOp.base.del(data->serial);
        data->serial = NULL;
        serialOK = False;
      }
      else {
        char* s = NULL;
        int timeout = 0;
        while( !SerialOp.available(data->serial) && timeout < 10 ) {
          ThreadOp.sleep(10);
          timeout++;
        }
        while( SerialOp.available(data->serial) ) {
          char b[2] = {'\0'};
          SerialOp.read( data->serial, &b[0], 1 );
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "read : %02X %c", b[0], b[0] );
          b[1] = '\0';
          if( b[0] != 0x0D && b[0] != 0x0A ) {
            s = StrOp.cat(s, b);
          }
          if( (b[0] == 0x0D || b[0] == 0x0A) && s != NULL ) {
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "read : %s", s );
            StrOp.free(s);
            s = NULL;
          }
          int timeout = 0;
          while( !SerialOp.available(data->serial) && timeout < 10 ) {
            ThreadOp.sleep(1);
            timeout++;
          }
        }
        if( s != NULL ) {
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "read : %s", s );
          StrOp.free(s);
          s = NULL;
        }
      }
      StrOp.free( cmd );
    }

    ThreadOp.sleep(10);
  };

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Transactor ended." );
}


/**  */
static struct ORaptor* _inst( const iONode ini ,const iOTrace trc ) {
  iORaptor __Raptor = allocMem( sizeof( struct ORaptor ) );
  iORaptorData data = allocMem( sizeof( struct ORaptorData ) );
  MemOp.basecpy( __Raptor, &RaptorOp, 0, sizeof( struct ORaptor ), data );

  /* Initialize data->xxx members... */
  TraceOp.set( trc );
  SystemOp.inst();

  data->ini    = ini;
  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );
  data->mux    = MutexOp.inst( NULL, True );
  data->run    = True;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Raptor %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  IID       : %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  device    : %s", wDigInt.getdevice( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );


  data->transactor = ThreadOp.inst( "raptortransactor", &__transactor, __Raptor );
  ThreadOp.start( data->transactor );

  instCnt++;
  return __Raptor;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/raptor.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
