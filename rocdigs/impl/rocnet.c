 /*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

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

#include "rocdigs/impl/rocnet_impl.h"
#include "rocdigs/impl/rocnet/rocnet-const.h"
#include "rocdigs/impl/rocnet/rocnet-parser.h"
#include "rocdigs/impl/rocnet/rn-utils.h"

#include "rocs/public/mem.h"
#include "rocs/public/objbase.h"
#include "rocs/public/string.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/RocNet.h"
#include "rocrail/wrapper/public/RocNetUDP.h"
#include "rocrail/wrapper/public/RocNetSerial.h"
#include "rocrail/wrapper/public/BinCmd.h"

#include "rocdigs/impl/common/fada.h"


static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOrocNetData data = Data(inst);
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


static byte __getProtocol(iONode loc) {
  byte prot = 0;

  if( StrOp.equals( wLoc.getprot(loc), wLoc.prot_N ) || StrOp.equals( wLoc.getprot(loc), wLoc.prot_L ) || StrOp.equals( wLoc.getprot(loc), wLoc.prot_P ) ) {
    if( wLoc.getspcnt(loc) < 28 )
      prot = RN_MOBILE_PROT_DCC14;
    if( wLoc.getspcnt(loc) > 28 )
      prot = RN_MOBILE_PROT_DCC128;
    else
      prot = RN_MOBILE_PROT_DCC28;
  }
  else if( StrOp.equals( wLoc.getprot(loc), wLoc.prot_M ) ) {
    prot = RN_MOBILE_PROT_MM1;

    if( wLoc.getprotver(loc) == 2 )
      prot = RN_MOBILE_PROT_MM2;
    else if( wLoc.getprotver(loc) == 3 )
      prot = RN_MOBILE_PROT_MM3;
    else if( wLoc.getprotver(loc) == 4 )
      prot = RN_MOBILE_PROT_MM4;
    else if( wLoc.getprotver(loc) == 5 )
      prot = RN_MOBILE_PROT_MM5;
  }

  return prot;
}

/** ----- OrocNet ----- */
static iONode __translate( iOrocNet inst, iONode node ) {
  iOrocNetData data = Data(inst);
  byte*  rn  = allocMem(32);;
  iONode rsp = NULL;

  rn[0] = RN_PACKET_START;

  if( data->extended )
    rnSenderAddresToPacket( wRocNet.getid(data->rnini), rn );


  /* BinCmd command. */
  if( StrOp.equals( NodeOp.getName( node ), wBinCmd.name() ) ) {
  }

  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );

    rn[RN_PACKET_GROUP] |= RN_GROUP_GENERAL;

    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      rn[RN_PACKET_ACTION] = RN_GENERAL_TRACKPOWER;
      rn[RN_PACKET_LEN] = 1;
      rn[RN_PACKET_DATA + 0] = RN_GENERAL_TRACKPOWER_OFF;
      ThreadOp.post( data->writer, (obj)rn );
      return rsp;
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      rn[RN_PACKET_ACTION] = RN_GENERAL_TRACKPOWER;
      rn[RN_PACKET_LEN] = 1;
      rn[RN_PACKET_DATA + 0] = RN_GENERAL_TRACKPOWER_ON;
      ThreadOp.post( data->writer, (obj)rn );
      return rsp;
    }
  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int addr   = wSwitch.getaddr1( node );
    int port   = wSwitch.getport1( node );
    int gate   = 0;
    int single = wSwitch.issinglegate( node );
    byte cmd   = 0;
    byte mask  = single ? 0x01:0x03;

    if( port == 0 )
      fromFADA( addr, &addr, &port, &gate );
    else if( addr == 0 && port > 0 )
      fromPADA( port, &addr, &port );

    addr = (addr-1) * 4 + port;

    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.straight ) ) {
      if( single ) cmd = 0x00;
      else         cmd = 0x01;
    }
    else {
      if( single ) cmd = 0x01;
      else         cmd = 0x02;
    }

    rn[RN_PACKET_GROUP] |= RN_GROUP_OUTPUT;
    rnAddresToPacket( addr, rn );
    rn[RN_PACKET_ACTION] = RN_OUTPUT_SWITCH_MULTI;
    rn[RN_PACKET_LEN] = 4;
    rn[RN_PACKET_DATA + 0] = 0;
    rn[RN_PACKET_DATA + 1] = mask;
    rn[RN_PACKET_DATA + 2] = 0;
    rn[RN_PACKET_DATA + 3] = cmd;
    ThreadOp.post( data->writer, (obj)rn );
    return rsp;
  }

  /* Signal command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSignal.name() ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "Signal commands are no longer supported at this level." );
  }

  /* Sensor command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFeedback.name() ) ) {
    int addr = wFeedback.getaddr( node );
    Boolean state = wFeedback.isstate( node );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "simulate fb addr=%d state=%s", addr, state?"true":"false" );
    rsp = (iONode)NodeOp.base.clone( node );
  }

  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int addr = wOutput.getaddr( node );
    int port = wOutput.getport( node );
    int gate = wOutput.getgate( node );
    byte cmd   = RN_OUTPUT_ON;

    if( port == 0 )
      fromFADA( addr, &addr, &port, &gate );
    else if( addr == 0 && port > 0 )
      fromPADA( port, &addr, &port );

    addr = (addr-1) * 4 + port;

    if( StrOp.equals( wOutput.getcmd( node ), wOutput.off ) ) {
      cmd = RN_OUTPUT_OFF;
    }

    rn[RN_PACKET_GROUP] |= RN_GROUP_OUTPUT;
    rnAddresToPacket( addr, rn );
    rn[RN_PACKET_ACTION] = RN_OUTPUT_SWITCH;
    rn[RN_PACKET_LEN] = 1;
    rn[RN_PACKET_DATA + 0] = cmd;
    ThreadOp.post( data->writer, (obj)rn );
    return rsp;
  }

  /* Loco command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int   addr = wLoc.getaddr( node );
    int      V = 0;
    byte    fn = wLoc.isfn( node )  ? RN_MOBILE_LIGHTS_ON:0;
    byte   dir = wLoc.isdir( node ) ? RN_MOBILE_DIR_FORWARDS:0;
    byte  prot = __getProtocol(node);

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        V = (wLoc.getV( node ) * 127) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        V = (wLoc.getV( node ) * 127) / wLoc.getV_max( node );
    }
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loc %d V=%d lights=%s dir=%s",
        addr, V, fn?"on":"off", dir?"forwards":"reverse" );

    rn[RN_PACKET_GROUP] |= RN_GROUP_MOBILE;
    rnAddresToPacket( addr, rn );
    rn[RN_PACKET_ACTION] = RN_MOBILE_VELOCITY;
    rn[RN_PACKET_LEN] = 2;
    rn[RN_PACKET_DATA + 0] = V;
    rn[RN_PACKET_DATA + 1] = dir | fn | prot;
    ThreadOp.post( data->writer, (obj)rn );
    return rsp;
  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int   addr = wFunCmd.getaddr( node );
    byte  prot = __getProtocol(node);
    Boolean fn   = wFunCmd.isf0(node);
    Boolean fn1  = wFunCmd.isf1(node);
    Boolean fn2  = wFunCmd.isf2(node);
    Boolean fn3  = wFunCmd.isf3(node);
    Boolean fn4  = wFunCmd.isf4(node);
    Boolean fn5  = wFunCmd.isf5(node);
    Boolean fn6  = wFunCmd.isf6(node);
    Boolean fn7  = wFunCmd.isf7(node);
    Boolean fn8  = wFunCmd.isf8(node);
    Boolean fn9  = wFunCmd.isf9(node);
    Boolean fn10 = wFunCmd.isf10(node);
    Boolean fn11 = wFunCmd.isf11(node);
    Boolean fn12 = wFunCmd.isf12(node);


    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "lc=%d lights=%s f1=%s f2=%s f3=%s f4=%s f5=%s f6=%s f7=%s f8=%s f9=%s f10=%s f11=%s f12=%s",
        addr, fn?"on":"off",
        fn1?"on":"off", fn2 ?"on":"off", fn3 ?"on":"off", fn4 ?"on":"off",
        fn5?"on":"off", fn6 ?"on":"off", fn7 ?"on":"off", fn8 ?"on":"off",
        fn9?"on":"off", fn10?"on":"off", fn11?"on":"off", fn12?"on":"off" );

    rn[RN_PACKET_GROUP] |= RN_GROUP_MOBILE;
    rnAddresToPacket( addr, rn );
    rn[RN_PACKET_ACTION] = RN_MOBILE_FUNCTIONS;
    rn[RN_PACKET_LEN] = 3;
    rn[RN_PACKET_DATA + 0] = (fn1?0x01:0x00) | (fn2?0x02:0x00) | (fn3?0x04:0x00) | (fn4?0x08:0x00) | (fn5?0x10:0x00) | (fn6?0x20:0x00) | (fn?0x40:0x00) ;
    rn[RN_PACKET_DATA + 1] = (fn7?0x01:0x00) | (fn8?0x02:0x00) | (fn9?0x04:0x00) | (fn10?0x08:0x00) | (fn11?0x10:0x00) | (fn12?0x20:0x00) ;
    rn[RN_PACKET_DATA + 2] = prot;
    ThreadOp.post( data->writer, (obj)rn );
    return rsp;
  }

  /* unhandled command */
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Unhandled command: [%s][%s]",
      NodeOp.getName(node), NodeOp.getStr(node, "cmd", "?") );
  /* not used; free up */
  freeMem(rn);

  return rsp;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOrocNetData data = Data(inst);
  iONode rsp = __translate( (iOrocNet)inst, cmd );

  /* Cleanup Node1 */
  cmd->base.del(cmd);

  return rsp;
}


/**  */
static void _halt( obj inst ) {
  iOrocNetData data = Data(inst);
  byte* rn;
  rn = allocMem(32);
  rn[0] = RN_PACKET_START;
  if( data->extended ) {
    rnSenderAddresToPacket( wRocNet.getid(data->rnini), rn );
    rn[RN_PACKET_EXT_GROUP] = RN_GROUP_GENERAL;
    rn[RN_PACKET_EXT_ACTION] = RN_GENERAL_TRACKPOWER;
    rn[RN_PACKET_EXT_LEN] = 1;
    rn[RN_PACKET_EXT_DATA + 0] = RN_GENERAL_TRACKPOWER_OFF;
  }
  else {
    rn[RN_PACKET_GROUP] |= RN_GROUP_GENERAL;
    rn[RN_PACKET_ACTION] = RN_GENERAL_TRACKPOWER;
    rn[RN_PACKET_LEN] = 1;
    rn[RN_PACKET_DATA + 0] = RN_GENERAL_TRACKPOWER_OFF;
  }
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
  ThreadOp.post( data->writer, (obj)rn );
  /* grab some time to process the request */
  ThreadOp.sleep(100);
  data->run = False;
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  return 0;
}


/** bit0=power, bit1=programming, bit2=connection */
static int _state( obj inst ) {
  return 0;
}


/* external shortcut event */
static void _shortcut(obj inst) {
  iOrocNetData data = Data( inst );
}



/**  */
static Boolean _supportPT( obj inst ) {
  return 0;
}


static void __evaluateStationary( iOrocNet rocnet, byte* rn ) {
  iOrocNetData data       = Data(rocnet);
  int          addr       = 0;
  int          rcpt       = 0;
  int          sndr       = 0;
  Boolean      isThis     = rocnetIsThis( rocnet, rn);
  int          action     = rnActionFromPacket(rn);
  int          actionType = rnActionTypeFromPacket(rn);

  if( data->extended ) {
    rcpt = rnReceipientAddrFromPacket(rn);
    sndr = rnSenderAddrFromPacket(rn);
  }
  else {
    addr = rnAddrFromPacket(rn);
  }

  switch( action ) {
  default:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unsupported action [%d]", action );
    break;
  }
}


static void __evaluatePTMobile( iOrocNet rocnet, byte* rn ) {
  iOrocNetData data       = Data(rocnet);
  int          addr       = 0;
  int          rcpt       = 0;
  int          sndr       = 0;
  Boolean      isThis     = rocnetIsThis( rocnet, rn);
  int          action     = rnActionFromPacket(rn);
  int          actionType = rnActionTypeFromPacket(rn);

  if( data->extended ) {
    rcpt = rnReceipientAddrFromPacket(rn);
    sndr = rnSenderAddrFromPacket(rn);
  }
  else {
    addr = rnAddrFromPacket(rn);
  }

  switch( action ) {
  default:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unsupported action [%d]", action );
    break;
  }
}


static void __evaluatePTStationary( iOrocNet rocnet, byte* rn ) {
  iOrocNetData data       = Data(rocnet);
  int          addr       = 0;
  int          rcpt       = 0;
  int          sndr       = 0;
  Boolean      isThis     = rocnetIsThis( rocnet, rn);
  int          action     = rnActionFromPacket(rn);
  int          actionType = rnActionTypeFromPacket(rn);

  if( data->extended ) {
    rcpt = rnReceipientAddrFromPacket(rn);
    sndr = rnSenderAddrFromPacket(rn);
  }
  else {
    addr = rnAddrFromPacket(rn);
  }

  switch( action ) {
  default:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unsupported action [%d]", action );
    break;
  }
}


static void __evaluateInput( iOrocNet rocnet, byte* rn ) {
  iOrocNetData data       = Data(rocnet);
  int          addr       = 0;
  int          rcpt       = 0;
  int          sndr       = 0;
  Boolean      isThis     = rocnetIsThis( rocnet, rn);
  int          action     = rnActionFromPacket(rn);
  int          actionType = rnActionTypeFromPacket(rn);

  if( data->extended ) {
    rcpt = rnReceipientAddrFromPacket(rn);
    sndr = rnSenderAddrFromPacket(rn);
  }
  else {
    addr = rnAddrFromPacket(rn);
  }

  switch( action ) {
  default:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unsupported action [%d]", action );
    break;
  }
}


static void __evaluateRN( iOrocNet rocnet, byte* rn ) {
  iOrocNetData data = Data(rocnet);
  int group = rn[RN_PACKET_GROUP] & RN_GROUP_MASK;
  byte* rnReply = NULL;

  switch( group ) {
    case RN_GROUP_GENERAL:
      rnReply = rocnetParseGeneral( rocnet, rn );
      break;

    case RN_GROUP_OUTPUT:
      rnReply = rocnetParseOutput( rocnet, rn );
      break;

    case RN_GROUP_INPUT:
      __evaluateInput( rocnet, rn );
      break;

    case RN_GROUP_MOBILE:
      rnReply = rocnetParseMobile( rocnet, rn );
      break;

    case RN_GROUP_STATIONARY:
      __evaluateStationary( rocnet, rn );
      break;

    case RN_GROUP_PT_MOBILE:
      __evaluatePTMobile( rocnet, rn );
      break;

    case RN_GROUP_PT_STATIONARY:
      __evaluatePTStationary( rocnet, rn );
      break;

    default:
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unsupported group [%d]", group );
      break;
  }

  if( rnReply != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "posting reply..." );
    ThreadOp.post( data->writer, (obj)rnReply );
  }
}


static void __reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOrocNet rocnet = (iOrocNet)ThreadOp.getParm( th );
  iOrocNetData data = Data(rocnet);
  char rn[0x7F];

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocNet reader started." );

  do {
    int extended = False;
    int event   = False;

    SocketOp.recvfrom( data->readUDP, rn, 0x7F );
    if( rnCheckPacket(rn, &extended, &event) )
      __evaluateRN( rocnet, rn );
    else
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "reject invalid packet" );

  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocNet reader stopped." );
}


static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOrocNet rocnet = (iOrocNet)ThreadOp.getParm( th );
  iOrocNetData data = Data(rocnet);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocNet writer started." );

  do {
    byte* rnRequest = (byte*)ThreadOp.getPost( th );
    if (rnRequest != NULL) {
      int extended = False;
      int event    = False;
      int plen     = 0;

      if( data->extended ) {
        plen = 8 + rnRequest[RN_PACKET_EXT_LEN];
      }
      else {
        plen = 5 + rnRequest[RN_PACKET_LEN];
      }

      if( rnCheckPacket(rnRequest, &extended, &event) ) {
        char* str = StrOp.byteToStr(rnRequest, plen);
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "write request from queue: [%s]", str );
        StrOp.free(str);
        SocketOp.sendto( data->writeUDP, rnRequest, plen );
      }
      freeMem( rnRequest);
    }

    ThreadOp.sleep(10);
  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocNet writer stopped." );
}



/* VERSION: */
static int vmajor = 0;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOrocNetData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}



/**  */
static struct OrocNet* _inst( const iONode ini ,const iOTrace trc ) {
  iOrocNet __rocNet = allocMem( sizeof( struct OrocNet ) );
  iOrocNetData data = allocMem( sizeof( struct OrocNetData ) );
  MemOp.basecpy( __rocNet, &rocNetOp, 0, sizeof( struct OrocNet ), data );

  TraceOp.set( trc );

  /* Initialize data->xxx members... */
  data->ini    = ini;
  data->rnini = wDigInt.getrocnet(ini);

  if( data->rnini == NULL ) {
    data->rnini = NodeOp.inst( wRocNet.name(), ini, ELEMENT_NODE );
    NodeOp.addChild( ini, data->rnini );
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocNET %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  type [%s]", wRocNet.gettype(data->rnini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  if( StrOp.equals( wRocNet.rn_type_udp, wRocNet.gettype(data->rnini) ) ) {
    iONode rnudp = wRocNet.getrnudp(data->rnini);
    if( rnudp == NULL ) {
      rnudp = NodeOp.inst( wRocNetUDP.name(), data->rnini, ELEMENT_NODE );
      NodeOp.addChild( data->rnini, rnudp );
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  multicast address [%s]", wRocNetUDP.getaddr(rnudp) );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  multicast port    [%d]", wRocNetUDP.getport(rnudp) );
    data->readUDP = SocketOp.inst( wRocNetUDP.getaddr(rnudp), wRocNetUDP.getport(rnudp), False, True );
    SocketOp.bind(data->readUDP);
    data->writeUDP = SocketOp.inst( wRocNetUDP.getaddr(rnudp), wRocNetUDP.getport(rnudp), False, True );
  }
  if( StrOp.equals( wRocNet.rn_type_serial, wRocNet.gettype(data->rnini) ) ) {
    iONode rnserial = wRocNet.getrnserial(data->rnini);
    if( rnserial == NULL ) {
      rnserial = NodeOp.inst( wRocNetSerial.name(), data->rnini, ELEMENT_NODE );
      NodeOp.addChild( data->rnini, rnserial );
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  serial device [%s]", wRocNetSerial.getdevice(rnserial) );
    data->serialCon = SerialOp.inst( wRocNetSerial.getdevice(rnserial) );
    SerialOp.setFlow( data->serialCon, cts );
    SerialOp.setLine( data->serialCon, 57600, 8, 2, none );
    SerialOp.open( data->serialCon );
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->run = True;


  data->reader = ThreadOp.inst( "rnreader", &__reader, __rocNet );
  ThreadOp.start( data->reader );

  data->writer = ThreadOp.inst( "rnwriter", &__writer, __rocNet );
  ThreadOp.start( data->writer );


  instCnt++;
  return __rocNet;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/rocnet.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
