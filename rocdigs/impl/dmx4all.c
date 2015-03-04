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

static void __setChannel(iODMX4ALL inst, int addr, int red, int green, int blue, int white, int white2, int brightness,
    Boolean active, int redChannel, int greenChannel, int blueChannel, int briChannel, int whiteChannel, int white2Channel)
{
  iODMX4ALLData data = Data(inst);
  if( MutexOp.wait( data->mux ) ) {
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999,
        "device=%d active=%d bri=%d RGBW=%d,%d,%d,%d", addr, active, brightness, red, green, blue, white );
    if( briChannel > 0 )
      data->dmxchannel[addr+briChannel-1] = brightness;
    else {
      red    = (red    * brightness) / 255;
      green  = (green  * brightness) / 255;
      blue   = (blue   * brightness) / 255;
      white  = (white  * brightness) / 255;
      white2 = (white2 * brightness) / 255;
    }

    if( redChannel > 0 ) {
      char* cmd = StrOp.fmt("C%03dL%03d", addr+redChannel-1, red);
      ThreadOp.post( data->transactor, (obj)cmd );
    }
    if( greenChannel > 0 ) {
      char* cmd = StrOp.fmt("C%03dL%03d", addr+greenChannel-1, green);
      ThreadOp.post( data->transactor, (obj)cmd );
    }
    if( blueChannel > 0 ) {
      char* cmd = StrOp.fmt("C%03dL%03d", addr+blueChannel-1, blue);
      ThreadOp.post( data->transactor, (obj)cmd );
    }
    if( whiteChannel > 0 ) {
      char* cmd = StrOp.fmt("C%03dL%03d", addr+whiteChannel-1, white);
      ThreadOp.post( data->transactor, (obj)cmd );
    }
    else if( data->mapwhite && white > 0 && redChannel > 0 && greenChannel > 0 && blueChannel > 0 ) {
      /* add it to the other channels */
      if( red + white > 255 ) {
        char* cmd = StrOp.fmt("C%03dL%03d", addr+redChannel-1, 255);
        ThreadOp.post( data->transactor, (obj)cmd );
      }
      else {
        char* cmd = StrOp.fmt("C%03dL%03d", addr+redChannel-1, red + white);
        ThreadOp.post( data->transactor, (obj)cmd );
      }

      if( green + white > 255 ) {
        char* cmd = StrOp.fmt("C%03dL%03d", addr+greenChannel-1, 255);
        ThreadOp.post( data->transactor, (obj)cmd );
      }
      else {
        char* cmd = StrOp.fmt("C%03dL%03d", addr+greenChannel-1, red + white);
        ThreadOp.post( data->transactor, (obj)cmd );
      }

      if( blue + white > 255 ) {
        char* cmd = StrOp.fmt("C%03dL%03d", addr+blueChannel-1, 255);
        ThreadOp.post( data->transactor, (obj)cmd );
      }
      else {
        char* cmd = StrOp.fmt("C%03dL%03d", addr+blueChannel-1, blue + white);
        ThreadOp.post( data->transactor, (obj)cmd );
      }

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "device %d does not support white; adjust other colors RGB=%d,%d,%d (base=%d,%d,%d)",
          addr, data->dmxchannel[addr+redChannel-1], data->dmxchannel[addr+greenChannel-1], data->dmxchannel[addr+blueChannel-1],
          red, green, blue );
    }
    if( white2Channel > 0 ) {
      char* cmd = StrOp.fmt("C%03dL%03d", addr+white2Channel-1, white2);
      ThreadOp.post( data->transactor, (obj)cmd );
    }


    MutexOp.post(data->mux);
  }
}



static iONode __translate( iODMX4ALL inst, iONode node ) {
  iODMX4ALLData data = Data(inst);
  iONode rsp   = NULL;

  /* Output command. */
  if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int addr = wOutput.getaddr( node );
    int port = wOutput.getport( node );
    int val  = wOutput.getvalue( node );
    iONode  color = wOutput.getcolor(node);
    Boolean blink = wOutput.isblink( node );
    Boolean colortype = wOutput.iscolortype( node );
    Boolean active = False;

    int redChannel    = wOutput.getredChannel(node);
    int greenChannel  = wOutput.getgreenChannel(node);
    int blueChannel   = wOutput.getblueChannel(node);
    int briChannel    = wOutput.getbrightnessChannel(node);
    int whiteChannel  = wOutput.getwhiteChannel(node);
    int white2Channel = wOutput.getwhite2Channel(node);
    int r  = 0;
    int g  = 0;
    int b  = 0;
    int w  = 0;
    int w2 = 0;

    if( StrOp.equals( wOutput.getcmd( node ), wOutput.on ) || StrOp.equals( wOutput.getcmd( node ), wOutput.value ) )
      active = True;

    if( color != NULL ) {
      r  = wColor.getred(color);
      g  = wColor.getgreen(color);
      b  = wColor.getblue(color);
      w  = wColor.getwhite(color);
      w2 = wColor.getwhite2(color);
    }
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output device=%d active=%d cmd=%s bri=%d briChannel(%d) RGBWW=%d,%d,%d,%d,%d RGBWWChannels(%d,%d,%d,%d,%d)",
        addr, active, wOutput.getcmd( node ), val, briChannel, r, g, b, w, w2, redChannel, greenChannel, blueChannel, whiteChannel, white2Channel );

    __setChannel(inst, addr, r, g, b, w, w2, val, active, redChannel, greenChannel, blueChannel, briChannel, whiteChannel, white2Channel);
  }

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
  Boolean   serialOK = False;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Transactor started." );

  ThreadOp.sleep(100);

  while( data->run ) {

    if( !serialOK ) {
      data->serial = SerialOp.inst( wDigInt.getdevice( data->ini ) );
      SerialOp.setFlow( data->serial, 0 );
      SerialOp.setLine( data->serial, 38400, 8, 1, none, wDigInt.isrtsdisabled( data->ini ) );
      SerialOp.setTimeout( data->serial, wDigInt.gettimeout(data->ini), wDigInt.gettimeout(data->ini) );
      serialOK = SerialOp.open( data->serial );
      if( !serialOK ) {
        SerialOp.base.del(data->serial);
        data->serial = NULL;
        ThreadOp.sleep(2500);
        continue;
      }
    }


    char* cmd = (char*)ThreadOp.getPost( th );
    if (cmd != NULL) {
      int len = cmd[0];
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "write: %s", cmd );
      if( !SerialOp.write( data->serial, cmd, StrOp.len(cmd) ) ) {
        SerialOp.base.del(data->serial);
        data->serial = NULL;
        serialOK = False;
      }
      else {
        while( SerialOp.available(data->serial) ) {
          char b = '\0';
          SerialOp.read( data->serial, &b, 1 );
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "read: %c", b );
        }
      }
      StrOp.free( cmd );
    }

    ThreadOp.sleep(10);
  };

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
