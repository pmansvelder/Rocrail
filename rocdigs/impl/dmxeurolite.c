/*
 Rocrail - Model Railroad Software

 Copyright (c) 2002 Robert Jan Versluis, Rocrail.net

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

#include "rocdigs/impl/dmxeurolite_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/system.h"
#include "rocs/public/strtok.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Color.h"
#include "rocrail/wrapper/public/DMX.h"

static int vmajor = 2;
static int vminor = 0;
static int patch  = 99;

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iODMXEUROLITEData data = Data(inst);
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

/** ----- ODMXEUROLITE ----- */
static void __setChannel(iODMXEUROLITE inst, int addr, int red, int green, int blue, int white, int white2, int brightness,
    Boolean active, int redChannel, int greenChannel, int blueChannel, int briChannel, int whiteChannel, int white2Channel)
{
  iODMXEUROLITEData data = Data(inst);
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
    data->dmxchannel[addr+redChannel-1] = red;
  }
  if( greenChannel > 0 ) {
    data->dmxchannel[addr+greenChannel-1] = green;
  }
  if( blueChannel > 0 ) {
    data->dmxchannel[addr+blueChannel-1] = blue;
  }
  if( whiteChannel > 0 ) {
    data->dmxchannel[addr+whiteChannel-1] = white;
  }
  if( white2Channel > 0 ) {
    data->dmxchannel[addr+white2Channel-1] = white2;
  }

  TraceOp.setDumpsize(NULL, 512);
  TraceOp.dump( name, TRCLEVEL_INFO, (char*)data->dmxchannel, 512 );

}





static iONode __translate( iODMXEUROLITE inst, iONode node ) {
  iODMXEUROLITEData data = Data(inst);
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

    __setChannel(inst, addr-1, r, g, b, w, w2, val, active, redChannel, greenChannel, blueChannel, briChannel, whiteChannel, white2Channel);
  }

  return rsp;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iODMXEUROLITEData data = Data(inst);
  iONode rsp = __translate( (iODMXEUROLITE)inst, cmd );
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
  iODMXEUROLITEData data = Data(inst);
  data->run = False;
  ThreadOp.sleep(600);
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
  iODMXEUROLITEData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static void __transactor( void* threadinst ) {
  iOThread        th = (iOThread)threadinst;
  iODMXEUROLITE  dmxeurolite = (iODMXEUROLITE)ThreadOp.getParm(th);
  iODMXEUROLITEData data = Data(dmxeurolite);
  Boolean   serialOK = False;
  Boolean   didInit  = False;
  byte buffer[520];

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Transactor started." );

  ThreadOp.sleep(100);

  while( data->run ) {

    if( !serialOK ) {
      data->serial = SerialOp.inst( wDigInt.getdevice( data->ini ) );
      SerialOp.setFlow( data->serial, cts );
      SerialOp.setLine( data->serial, 115200, 8, 1, none, wDigInt.isrtsdisabled( data->ini ) );
      SerialOp.setTimeout( data->serial, wDigInt.gettimeout(data->ini), wDigInt.gettimeout(data->ini) );
      serialOK = SerialOp.open( data->serial );
      if( !serialOK ) {
        SerialOp.base.del(data->serial);
        data->serial = NULL;
        ThreadOp.sleep(2500);
        continue;
      }
      buffer[0] = 0x7E;
      buffer[1] = 0x0A; /* request serial number... */
      buffer[2] = 0x00;
      buffer[3] = 0x00;
      buffer[4] = 0xE7;
      TraceOp.dump ( name, TRCLEVEL_BYTE, (char*)buffer, 5 );
      SerialOp.write( data->serial, (char*)buffer, 5 );
    }

    // The only understood message is label 6: in Hex: 7E 06 01 02 00 [512 * Channel Data] E7
    buffer[0] = 0x7E;
    buffer[1] = 0x06;
    buffer[2] = 0x01;
    buffer[3] = 0x02;
    buffer[4] = 0x00;
    MemOp.copy(buffer+5, data->dmxchannel, 512);
    buffer[5+512] = 0xE7;
    //TraceOp.setDumpsize(NULL, 520);
    //TraceOp.dump ( name, TRCLEVEL_INFO, (char*)buffer, 5 + 512 + 1 );
    if( !SerialOp.write( data->serial, (char*)buffer, 5 + 512 + 1 ) ) {
      SerialOp.base.del(data->serial);
      data->serial = NULL;
      serialOK = False;
    }

    if( serialOK ) {
      while( SerialOp.available(data->serial) ) {
        char b[2] = {'\0'};
        SerialOp.read( data->serial, &b[0], 1 );
        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "read : %02X", b[0] );
      }
    }

    ThreadOp.sleep(500);
  };

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Transactor ended." );
}

/**  */
static struct ODMXEUROLITE* _inst( const iONode ini ,const iOTrace trc ) {
  iODMXEUROLITE __DMXEUROLITE = allocMem( sizeof( struct ODMXEUROLITE ) );
  iODMXEUROLITEData data = allocMem( sizeof( struct ODMXEUROLITEData ) );
  MemOp.basecpy( __DMXEUROLITE, &DMXEUROLITEOp, 0, sizeof( struct ODMXEUROLITE ), data );

  /* Initialize data->xxx members... */
  TraceOp.set( trc );
  SystemOp.inst();

  data->ini    = ini;
  data->dmxini = wDigInt.getdmx(ini);
  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );
  data->run    = True;

  if( data->dmxini == NULL ) {
    data->dmxini = NodeOp.inst(wDMX.name(), data->ini, ELEMENT_NODE);
    NodeOp.addChild(data->ini, data->dmxini);
  }

  data->mapwhite  = wDMX.ismapwhite(data->dmxini);
  data->framerate = 30;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "DMXEUROLITE %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  IID       : %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  device    : %s", wDigInt.getdevice( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->transactor = ThreadOp.inst( "dmxprotransactor", &__transactor, __DMXEUROLITE );
  ThreadOp.start( data->transactor );

  instCnt++;
  return __DMXEUROLITE;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/dmxeurolite.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
