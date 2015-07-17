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




#include "rocdigs/impl/dmxartnet_impl.h"

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
    iODMXArtNetData data = Data(inst);
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

/** ----- ODMXArtNet ----- */

#define ARTDMX_MSGSIZE 531
#define ARTPOL_MSGSIZE 8+7
#define ARTPOL_IP "255.255.255.255"
#define ARTPOL_PORT 6454

static int __ArtHeader(byte* msg) {
  int i = 0;
  msg[i++]='A';
  msg[i++]='r';
  msg[i++]='t';
  msg[i++]='-';
  msg[i++]='N';
  msg[i++]='e';
  msg[i++]='t';
  msg[i++]=0x00;
  return i;
}


/*
    00000000: 41 72 74 2D 4E 65 74 00 00 21 C0 A8 64 5B 36 19 |Art-Net..!..d[6.|
    00000010: 03 10 00 00 08 B1 00 00 4B 53 41 76 72 41 72 74 |........KSAvrArt|
    00000020: 4E 6F 64 65 00 00 00 00 00 00 00 00 41 56 52 20 |Node........AVR |
    00000030: 62 61 73 65 64 20 41 72 74 2D 4E 65 74 20 6E 6F |based Art-Net no|
    00000040: 64 65 00 00 00 00 00 00 00 00 00 00 00 00 00 00 |de..............|
    00000050: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 |................|
    00000060: 00 00 00 00 00 00 00 00 00 00 00 00 41 76 72 41 |............AvrA|
    00000070: 72 74 4E 6F 64 65 20 69 73 20 72 65 61 64 79 00 |rtNode is ready.|
 */
static void __ArtPollReply(iODMXArtNet inst, iOSocket socket) {
  iODMXArtNetData data = Data(inst);

  byte msg[ARTDMX_MSGSIZE];
  int i = __ArtHeader(msg);

  msg[i++]=0; // OpArtPollReply 0x2100
  msg[i++]=0x21;

  const char* ip = SocketOp.gethostaddr();
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "IP address [%s]", ip);
  iOStrTok tok = StrTokOp.inst( ip, '.' );
  if( StrTokOp.hasMoreTokens(tok) )
    msg[i] = atoi(StrTokOp.nextToken( tok ));
  i++;
  if( StrTokOp.hasMoreTokens(tok) )
    msg[i] = atoi(StrTokOp.nextToken( tok ));
  i++;
  if( StrTokOp.hasMoreTokens(tok) )
    msg[i] = atoi(StrTokOp.nextToken( tok ));
  i++;
  if( StrTokOp.hasMoreTokens(tok) )
    msg[i] = atoi(StrTokOp.nextToken( tok ));
  i++;
  StrTokOp.base.del(tok);

  msg[14] = ARTPOL_PORT % 256;
  msg[15] = ARTPOL_PORT / 256;

  msg[16] = vmajor;
  msg[17] = vminor;

  // Rocrail OEM 0x1180
  msg[20] = 0x11;
  msg[21] = 0x80;

  msg[24] = 'R';
  msg[25] = 'R';

  StrOp.copy((char*)(msg+26), "Rocrail");
  StrOp.copy((char*)(msg+45), "Rocrail DMX Daylight");
  StrOp.copy((char*)(msg+108), "Ready and running");

  if( socket != NULL ) {
    SocketOp.sendto( socket, (char*)msg, 207, NULL, 0 );
  }
}

static void __ArtPoll(iODMXArtNet inst, iOSocket socket) {
  iODMXArtNetData data = Data(inst);

  byte msg[ARTPOL_MSGSIZE];
  int i = __ArtHeader(msg);

  msg[i++]=0; // OpArtPol 0x2000
  msg[i++]=0x20;

  msg[i++]=0x50; //version
  msg[i++]=0x0E;

  msg[i++]=0x06; // TalkToMe
  msg[i++]=0x00; // Priority

  if( socket != NULL ) {

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "ArtPol broadcast...");

    TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)msg, i );
    SocketOp.sendto( socket, (char*)msg, i, NULL, 0 );
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "ArtPol broadcast ended");
  }
}

static void __ArtDmx(iODMXArtNet inst) {
  iODMXArtNetData data = Data(inst);

  byte msg[ARTDMX_MSGSIZE];
  int i = __ArtHeader(msg);

  msg[i++]=0; // OpOutput 0x5000
  msg[i++]=0x50;

  msg[i++]=0x50; //version
  msg[i++]=0x0E;

  msg[i++]=0x00; //sequence =0

  msg[i++]=0x00; //physical

  msg[i++]=0x00;
  msg[i++]=0x00; //subuni net

  msg[i++]=0x02;
  msg[i]=0x00;

  if( MutexOp.wait( data->mux ) ) {
    // add DMX-values
    int j = 0;
    for(j = 1; j <= 512; j++) {
      msg[i+j]=(byte)data->dmxchannel[j];
    }
    MutexOp.post(data->mux);
  }

  iOSocket socket = SocketOp.inst( wDigInt.gethost(data->ini), 6454, False, True, False );
  if( socket != NULL ) {
    TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)msg, ARTDMX_MSGSIZE );
    SocketOp.sendto( socket, (char*)msg, ARTDMX_MSGSIZE, NULL, 0 );
    SocketOp.base.del(socket);
  }

}

static void __setChannel(iODMXArtNet inst, int addr, int red, int green, int blue, int white, int white2, int brightness,
    Boolean active, int redChannel, int greenChannel, int blueChannel, int briChannel, int whiteChannel, int white2Channel)
{
  iODMXArtNetData data = Data(inst);
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

    if( redChannel > 0 )
      data->dmxchannel[addr+redChannel-1] = red;
    if( greenChannel > 0 )
      data->dmxchannel[addr+greenChannel-1] = green;
    if( blueChannel > 0 )
      data->dmxchannel[addr+blueChannel-1] = blue;
    if( whiteChannel > 0 )
      data->dmxchannel[addr+whiteChannel-1] = white;
    else if( data->mapwhite && white > 0 && redChannel > 0 && greenChannel > 0 && blueChannel > 0 ) {
      /* add it to the other channels */
      if( red + white > 255 )
        data->dmxchannel[addr+redChannel-1] = 255;
      else
        data->dmxchannel[addr+redChannel-1] = red + white;

      if( green + white > 255 )
        data->dmxchannel[addr+greenChannel-1] = 255;
      else
        data->dmxchannel[addr+greenChannel-1] = green + white;

      if( blue + white > 255 )
        data->dmxchannel[addr+blueChannel-1] = 255;
      else
        data->dmxchannel[addr+blueChannel-1] = blue + white;

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "device %d does not support white; adjust other colors RGB=%d,%d,%d (base=%d,%d,%d)",
          addr, data->dmxchannel[addr+redChannel-1], data->dmxchannel[addr+greenChannel-1], data->dmxchannel[addr+blueChannel-1],
          red, green, blue );
    }
    if( white2Channel > 0 )
      data->dmxchannel[addr+white2Channel-1] = white2;


    MutexOp.post(data->mux);
  }
}


static iONode __translate( iODMXArtNet inst, iONode node ) {
  iODMXArtNetData data = Data(inst);
  iONode rsp   = NULL;

  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );

    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
    }
  }

  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
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
    data->refresh = True;
  }



  return rsp;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iODMXArtNetData data = Data(inst);
  iONode rsp = __translate( (iODMXArtNet)inst, cmd );
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
  iODMXArtNetData data = Data(inst);
  data->run = False;
  if( poweroff ) {
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
  char* dmxdata = StrOp.byteToStr(data->dmxchannel, 513);
  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "save data:\n%s", dmxdata );
  wDMX.setdmxdata(data->dmxini, dmxdata);
  StrOp.free(dmxdata);
  ThreadOp.sleep(200);
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iODMXArtNetData data = Data(inst);
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
  iODMXArtNetData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iODMXArtNet inst = (iODMXArtNet)ThreadOp.getParm( th );
  iODMXArtNetData data = Data(inst);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "writer started." );
  ThreadOp.sleep(1000);

  int framepause = 0;
  while( data->run ) {
    ThreadOp.sleep(10);
    framepause += 10;
    if( framepause >= data->framerate || data->refresh) {
      data->refresh = False;
      framepause = 0;
      __ArtDmx(inst);
    }
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "writer ended." );
}


/*
20150120.122246.136 r9999I dmxreade OSocket  0752 207 bytes readed from 192.168.100.91:6454
20150120.122246.136 r0000I dmxreade (null)   *trace dump( 0x03E02B70: length=128 )
    offset:   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |ASCII...........|
    --------------------------------------------------------- |----------------|
    00000000: 41 72 74 2D 4E 65 74 00 00 21 C0 A8 64 5B 36 19 |Art-Net..!..d[6.|
    00000010: 03 10 00 00 08 B1 00 00 4B 53 41 76 72 41 72 74 |........KSAvrArt|
    00000020: 4E 6F 64 65 00 00 00 00 00 00 00 00 41 56 52 20 |Node........AVR |
    00000030: 62 61 73 65 64 20 41 72 74 2D 4E 65 74 20 6E 6F |based Art-Net no|
    00000040: 64 65 00 00 00 00 00 00 00 00 00 00 00 00 00 00 |de..............|
    00000050: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 |................|
    00000060: 00 00 00 00 00 00 00 00 00 00 00 00 41 76 72 41 |............AvrA|
    00000070: 72 74 4E 6F 64 65 20 69 73 20 72 65 61 64 79 00 |rtNode is ready.|

 */
#define OPCODE_ArtPoll 0x2000
#define OPCODE_ArtPollReply 0x2100

static void __evaluateArtNet(iODMXArtNet inst, iOSocket socket, byte* msg) {
  iODMXArtNetData data = Data(inst);
  int opcode = msg[8] + msg[9] * 256;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "opcode = 0x%02X", opcode );
  if(opcode == OPCODE_ArtPoll) {
    __ArtPollReply(inst, socket);
  }
  else if(opcode == OPCODE_ArtPollReply) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "ArtPollReply %d.%d.%d.%d:%d version=%d.%d OEM=%d manu=%c%c name=[%s, %s] state=[%s]",
        msg[10], msg[11], msg[12], msg[13], msg[14] + msg[15]*256, msg[16], msg[17], (msg[20]*256 + msg[21])&0x7FFF, msg[24], msg[25], msg+26, msg+45, msg+108 );
  }
}


static void __reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iODMXArtNet inst = (iODMXArtNet)ThreadOp.getParm( th );
  iODMXArtNetData data = Data(inst);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reader started." );
  ThreadOp.sleep(500);

  iOSocket socket = SocketOp.inst( ARTPOL_IP, 6454, False, True, True );
  if( socket != NULL ) {
    char client[256] = {'\0'};
    int port = 0;
    byte in[ARTDMX_MSGSIZE];
    SocketOp.setBroadcast(socket, True);
    SocketOp.bind(socket);
    __ArtPoll(inst, socket);
    while( data->run ) {
      int packetSize = SocketOp.recvfrom( socket, (char*)in, ARTDMX_MSGSIZE, client, &port );

      if( packetSize > 0 ) {
        TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)in, packetSize );
        __evaluateArtNet(inst, socket, in);
        MemOp.set(in, 0, ARTDMX_MSGSIZE);
      }
      ThreadOp.sleep(100);
    }
    SocketOp.base.del(socket);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reader ended." );
}


/**  */
static struct ODMXArtNet* _inst( const iONode ini ,const iOTrace trc ) {
  iODMXArtNet __DMXArtNet = allocMem( sizeof( struct ODMXArtNet ) );
  iODMXArtNetData data = allocMem( sizeof( struct ODMXArtNetData ) );
  MemOp.basecpy( __DMXArtNet, &DMXArtNetOp, 0, sizeof( struct ODMXArtNet ), data );

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

  if( wDMX.isrestoredata(data->dmxini) ) {
    byte* dmxdata = StrOp.strToByte( wDMX.getdmxdata(data->dmxini) );
    if( dmxdata != NULL ) {
      int len = StrOp.len(wDMX.getdmxdata(data->dmxini))/2;
      MemOp.copy(data->dmxchannel, dmxdata, len);
      freeMem(dmxdata);
    }
  }


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "DMX-ArtNet %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  IID       : %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  IP        : %s", wDigInt.gethost(data->ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  Frame rate: %d", data->framerate );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->reader = ThreadOp.inst( "dmxreader", &__reader, __DMXArtNet );
  ThreadOp.start( data->reader );

  data->writer = ThreadOp.inst( "dmxwriter", &__writer, __DMXArtNet );
  ThreadOp.start( data->writer );

  instCnt++;
  return __DMXArtNet;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/dmxartnet.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
