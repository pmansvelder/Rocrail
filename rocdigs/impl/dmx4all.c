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



#include "rocdigs/impl/dmx4all_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/system.h"
#include "rocs/public/strtok.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Color.h"
#include "rocrail/wrapper/public/DMX.h"


static int instCnt = 0;

static int vmajor = 2;
static int vminor = 0;
static int patch  = 99;


/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iODMX4ALLData data = Data(inst);
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

/** ----- ODMX4ALL ----- */

static iONode __translate( iODMX4ALL inst, iONode node ) {
  iODMX4ALLData data = Data(inst);
  iONode rsp   = NULL;


  return rsp;
}

/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iODMX4ALLData data = Data(inst);
  iONode rsp = __translate( (iODMX4ALL)inst, cmd );
  /* Cleanup Node1 */
  cmd->base.del(cmd);
  return rsp;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ,Boolean shutdown ) {
  iODMX4ALLData data = Data(inst);
  data->run = False;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  return 0;
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
  iODMX4ALLData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static void __transactor( void* threadinst ) {
  iOThread        th = (iOThread)threadinst;
  iODMX4ALL  dmx4all = (iODMX4ALL)ThreadOp.getParm(th);
  iODMX4ALLData data = Data(dmx4all);

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Transactor started." );

  ThreadOp.sleep(100);
  data->serial = SerialOp.inst( wDigInt.getdevice( data->ini ) );
  SerialOp.setFlow( data->serial, 0 );
  SerialOp.setLine( data->serial, 38400, 8, 1, none, False );


  do {
    obj post = NULL;

    post = ThreadOp.getPost( th );

    if( post != NULL ) {
      char* cmd = (char*)post;

      StrOp.free(cmd);
    }

    ThreadOp.sleep(10);
  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Transactor ended." );
}


/**  */
static struct ODMX4ALL* _inst( const iONode ini ,const iOTrace trc ) {
  iODMX4ALL __DMX4ALL = allocMem( sizeof( struct ODMX4ALL ) );
  iODMX4ALLData data = allocMem( sizeof( struct ODMX4ALLData ) );
  MemOp.basecpy( __DMX4ALL, &DMX4ALLOp, 0, sizeof( struct ODMX4ALL ), data );

  /* Initialize data->xxx members... */
  TraceOp.set( trc );
  SystemOp.inst();

  data->ini    = ini;
  data->dmxini = wDigInt.getdmx(ini);
  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );
  data->mux    = MutexOp.inst( NULL, True );
  data->run    = True;

  if( data->dmxini == NULL ) {
    data->dmxini = NodeOp.inst(wDMX.name(), data->ini, ELEMENT_NODE);
    NodeOp.addChild(data->ini, data->dmxini);
  }

  data->mapwhite  = wDMX.ismapwhite(data->dmxini);
  data->framerate = wDMX.getframerate(data->dmxini);
  if( data->framerate < 100 )
    data->framerate = 500;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "DMX4ALL %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  IID       : %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  device    : %s", wDigInt.getdevice( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->transactor = ThreadOp.inst( "dmxtransactor", &__transactor, __DMX4ALL );
  ThreadOp.start( data->transactor );

  instCnt++;
  return __DMX4ALL;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/dmx4all.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
