/*
 Rocrail - Model Railroad Control System
 Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net
               2014-2015 Bert Havinga
 All rights reserved.
*/

/** ------------------------------------------------------------
  * Module: RocDigs
  * Object: MGBOX using MCS2 wrapper
  * ------------------------------------------------------------
  */
/*
 * CAN over TCP/IP 13 byte format:
 *
 *  |  0   |  1    | 2 | 3 |  4  | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 |
 *  | PRIO CMD RSP | HASH  | DLC |             DATA                 |
 *
 *
 *
 *
 */

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "rocdigs/impl/mcs2_impl.h"
#include "rocdigs/impl/mcs2/mcs2-const.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/system.h"


#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/MCS2.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/LocList.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/BinCmd.h"
#include "rocrail/wrapper/public/Product.h"

#include "rocutils/public/addr.h"
#include "common/version.h"


static int instCnt = 0;
static int rrUID = 0;
static int rrHash = 0x2F20;
static char idname[32] = { '\0' }; /* mfx loc readable name */
static Boolean firstview = True;
static byte reportstatechannel[3] = {1,3,4};
static byte devconfig[48] = { 0 };
static char devstrbuffer[32] = { '\0' };
static int ms2UIDs[32] = { 0 };
static int ms2hash[32] = { 0 };


static void __reportState(iOMCS2Data data);


/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOMCS2Data data = Data(inst);
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

/** ----- OMCS2 ----- */
static byte* __makeMsg( int prio, int cmd, Boolean rsp, int len, byte* buffer ) {
  int i = 0;
  byte* msg = allocMem(32);
  msg[0]  = (prio << 1);
  msg[0] |= (cmd >> 7);
  msg[1]  = ((cmd & 0x7F) << 1 );
  msg[1] |= rsp;
  msg[2]  = (rrHash / 256) & 0xFF;
  msg[3]  = (rrHash % 256) & 0xFF;
  msg[4]  = len;
  if( cmd == CAN_CMD_PING ) len = 8; /* however a ping cmd is length == 0, the system uses the UID to determine a valid packet by computing the hash from the UID */ 
  for(i = 0; i < len; i++ )
    msg[5+i]  = buffer[i];
  return msg;
}


static Boolean __getFunctionState(iONode node, int fnchanged) {
  switch( fnchanged ) {
  case 0: return wFunCmd.isf0(node);
  case 1: return wFunCmd.isf1(node);
  case 2: return wFunCmd.isf2(node);
  case 3: return wFunCmd.isf3(node);
  case 4: return wFunCmd.isf4(node);
  case 5: return wFunCmd.isf5(node);
  case 6: return wFunCmd.isf6(node);
  case 7: return wFunCmd.isf7(node);
  case 8: return wFunCmd.isf8(node);
  case 9: return wFunCmd.isf9(node);
  case 10: return wFunCmd.isf10(node);
  case 11: return wFunCmd.isf11(node);
  case 12: return wFunCmd.isf12(node);
  case 13: return wFunCmd.isf13(node);
  case 14: return wFunCmd.isf14(node);
  case 15: return wFunCmd.isf15(node);
  case 16: return wFunCmd.isf16(node);
  case 17: return wFunCmd.isf17(node);
  case 18: return wFunCmd.isf18(node);
  case 19: return wFunCmd.isf19(node);
  case 20: return wFunCmd.isf20(node);
  case 21: return wFunCmd.isf21(node);
  case 22: return wFunCmd.isf22(node);
  case 23: return wFunCmd.isf23(node);
  case 24: return wFunCmd.isf24(node);
  case 25: return wFunCmd.isf25(node);
  case 26: return wFunCmd.isf26(node);
  case 27: return wFunCmd.isf27(node);
  case 28: return wFunCmd.isf28(node);
  }
  return False;
}


static void __setSysMsg( byte* msg, int prio, int cmd, Boolean rsp, int len, long addr, int subcmd, int subcmd2, int subcmd3, int subcmd4 ) {
  msg[0]  = (prio << 1);
  msg[0] |= (cmd >> 7);
  msg[1]  = ((cmd & 0x7F) << 1 );
  msg[1] |= rsp;
  msg[2]  = (rrHash / 256) & 0xFF;
  msg[3]  = (rrHash % 256) & 0xFF;
  msg[4]  = len;
  msg[5]  = (addr & 0xFF000000) >> 24;
  msg[6]  = (addr & 0x00FF0000) >> 16;
  msg[7]  = (addr & 0x0000FF00) >> 8;
  msg[8]  = (addr & 0x000000FF);
  msg[9]  = subcmd;
  msg[10] = subcmd2;
  msg[11] = subcmd3;
  msg[12] = subcmd4;
}


static void __SoD( iOMCS2 inst ) {
  iOMCS2Data data = Data(inst);

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Start of Day..." );

  if( data->fbmod > 0 && data->fbmod > data->sod_fbmod) {

    if( wDigInt.getprotver( data->ini ) >= 3 ) {
      long devID = 0;
      int i = 0;
      for( i = 0; i < data->fbmod * 16; i++ ) {
        byte* out = allocMem(16);
        __setSysMsg(out, 0, 0x11, False, 4, devID + 1 + i, 0, 0, 0, 0);
        ThreadOp.post( data->writer, (obj)out );
        ThreadOp.sleep(20);
      }
    }
    else {
      long devID = 0;
      byte* out = allocMem(16);
      __setSysMsg(out, 0, 0x10, False, 5, devID, data->fbmod, 0, 0, 0);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Query %d S88 sensor modules...", data->fbmod );
      ThreadOp.post( data->writer, (obj)out );
    }

    data->sod_fbmod = data->fbmod;
  }

  ThreadOp.sleep(100);

  if( wDigInt.getprotver( data->ini ) >= 2 && (data->sensorend - data->sensorbegin) > 0 ) {
    if(data->sensorbegin < data->sod_sensorbegin || data->sensorend > data->sod_sensorend) {
      byte*  msg   = allocMem(32);
      msg[0] = (CMD_ACC_SENSOR >> 7);
      msg[1]  = ((CMD_ACC_SENSOR & 0x7F) << 1 );
      msg[2]  = (rrHash / 256) & 0xFF;
      msg[3]  = (rrHash % 256) & 0xFF;
      msg[4]  = 7;
      msg[5]  = wMCS2.getfbdevid(data->mcs2ini) / 256; /* Geraetekenner */
      msg[6]  = wMCS2.getfbdevid(data->mcs2ini) % 256;
      msg[7]  = (data->sensorbegin & 0xFF00) >> 8; /* Kontaktkennung Start */
      msg[8]  = (data->sensorbegin & 0x00FF);
      msg[9]  = (data->sensorend & 0xFF00) >> 8; /* Kontaktkennung Ende */
      msg[10] = (data->sensorend & 0x00FF);
      msg[11] = 0xFF; /* Broadcast ein */
      msg[12] = 0;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Query sensors from %d to %d for version >= 2...", data->sensorbegin, data->sensorend );
      ThreadOp.post( data->writer, (obj)msg );

      data->sod_sensorbegin = data->sensorbegin;
      data->sod_sensorend   = data->sensorend;
    }
  }
}


static void __sendMCS2PingCmd( iOMCS2Data data ) {
  const int sysgerken = 0xFF20;
    byte  buffer[32];
    buffer[0]  = (rrUID  & 0xFF000000) >> 24;
    buffer[1]  = (rrUID  & 0x00FF0000) >> 16;
    buffer[2]  = (rrUID  & 0x0000FF00) >> 8;
    buffer[3]  = (rrUID  & 0x000000FF);
    buffer[4]  = (revisionnr / 100) & 0xFF;
    buffer[5]  = (revisionnr % 100) & 0xFF;
    buffer[6]  = (sysgerken / 256) & 0xFF;
    buffer[7]  = (sysgerken % 256) & 0xFF;
    ThreadOp.post( data->writer, (obj)__makeMsg(0, CAN_CMD_PING, False, 0, buffer) );
}


static iONode __translate( iOMCS2 inst, iONode node ) {
  iOMCS2Data data = Data(inst);
  iONode rsp   = NULL;

  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );

    if( StrOp.equals( cmd, wSysCmd.stop ) && data->power ) {
      byte* out = allocMem(32);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "System STOP to %s", ( data->gbUID ? "Gleisbox" : "CS2" ) );
      data->power = False;
      __setSysMsg(out, 0, CMD_SYSTEM, False, 5, 0, CMD_SYSSUB_STOP, 0, 0, 0);
      ThreadOp.post( data->writer, (obj)out );
      __reportState(data);
      return rsp;
    }
    else if( StrOp.equals( cmd, wSysCmd.ebreak ) ) {
      byte* out = allocMem(32);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "System HALT to %s", ( data->gbUID ? "Gleisbox" : "CS2" ) );
      __setSysMsg(out, 0, CMD_SYSTEM, False, 5, 0, CMD_SYSSUB_HALT, 0, 0, 0);
      ThreadOp.post( data->writer, (obj)out );
      return rsp;
    }
    else if( StrOp.equals( cmd, wSysCmd.go )  && !data->power ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Request CAN configuration by \"Go\"" );
      __sendMCS2PingCmd(data);

      byte* out = allocMem(32);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "System GO to %s", ( data->gbUID ? "Gleisbox" : "CS2" ) );
      data->power = True;
      __setSysMsg(out, 0, CMD_SYSTEM, False, 5, 0, CMD_SYSSUB_GO, 0, 0, 0);
      ThreadOp.post( data->writer, (obj)out );
      __reportState(data);
      return rsp;
    }
    else if( StrOp.equals( cmd, wSysCmd.sod ) ) {
      data->sod_fbmod = 0;
      data->sod_sensorbegin = 0;
      data->sod_sensorend = 0;
      __SoD(inst);

      return rsp;
    }
  }

  /* Switch command */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    byte* out = allocMem(32);
    int module = wSwitch.getaddr1( node );
    if ( module == 0 )
      /* pada used, port will be actual cs2 lineair address */
      module = 1;

    int port = wSwitch.getport1( node );
    int gate = wSwitch.getgate1( node );
    int swtime = wSwitch.getdelay( node ) / 10;
    int action = 1;

    if( swtime == 0 ) {
      swtime = data->swtime / 10;
    }

    Boolean dccswitch = StrOp.equals( wSwitch.getprot( node ), wSwitch.prot_N );
    if( port == 0 )
      AddrOp.fromFADA( module, &module, &port, &gate );

    long address = (( module - 1 ) * 4 ) + port - 1 + (dccswitch?0x3800:0x3000);
    /* cs 2 uses lineair addressing, address range 0x3000-0x33ff is for accessory decoders MM, 0x3800 for DCC, CS2 first address is 0, Rocrail's first is 1 */

    if ( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout )) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Switch %d (%s) to turnout", (address - (dccswitch?0x37FF:0x2FFF) ), dccswitch?"dcc":"mm" );
      if( wSwitch.issinglegate( node ) )
        action = 1;
      else
        gate = 0;
    }
    else {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Switch %d (%s) to straight", (address - (dccswitch?0x37FF:0x2FFF) ), dccswitch?"dcc":"mm" );
      if( wSwitch.issinglegate( node ) )
        action = 0;
      else
        gate = 1;
    }
    __setSysMsg(out, 0, CMD_ACC_SWITCH, False, 8, address, gate, action, swtime/256, swtime%256);

    ThreadOp.post( data->writer, (obj)out );

    {
      int addr = wSwitch.getaddr1( node );
      int port = wSwitch.getport1( node );
      int gate = wSwitch.getgate1( node );
      iONode nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );

      if( port == 0 && addr > 0 )
        AddrOp.fromFADA( addr, &addr, &port, &gate );
      else if( addr == 0 && port > 0 )
        AddrOp.fromPADA( port, &addr, &port );

      wSwitch.setbus( nodeC, wSwitch.getbus( node ) );
      wSwitch.setaddr1( nodeC, wSwitch.getaddr1( node ) );
      wSwitch.setport1( nodeC, wSwitch.getport1( node ) );
      wSwitch.setgate1( nodeC, wSwitch.getgate1( node ) );
      wSwitch.setgatevalue( nodeC, action );

      if( wSwitch.getiid(node) != NULL )
        wSwitch.setiid( nodeC, wSwitch.getiid(node) );

      wSwitch.setstate( nodeC, wSwitch.getcmd( node ) );

      data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
    }

    return rsp;
  }

  /* Signal command */
  else if( StrOp.equals( NodeOp.getName(node), wSignal.name() ) ) {
    byte* out = allocMem(32);
    int module = wSignal.getaddr(node);
    int port   = wSignal.getport1(node);
    int gate   = wSignal.getgate1(node);
    int aspect = wSignal.getaspect(node);
    if ( module == 0 )
      module = 1;

    Boolean dccoutput = StrOp.equals( wSignal.getprot( node ), wOutput.prot_N );
    if( port == 0 )    /* fada used convert to address, port */
      AddrOp.fromFADA( module, &module, &port, &gate );

    long address = (( module - 1 ) * 4 ) + port -1 + (dccoutput?0x3800:0x3000);
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Signal %d (%s) aspect=%d", (address - (dccoutput?0x37FF:0x2FFF) ), dccoutput?"dcc":"mm", aspect );
    __setSysMsg(out, 0, CMD_ACC_SWITCH, False, 8, address, aspect, 1, 0, 0);
    ThreadOp.post( data->writer, (obj)out );
    return rsp;
  }

  /* Output command */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    byte* out = allocMem(32);
    int swtime = data->swtime / 10;
    int module = wOutput.getaddr( node );
    if ( module == 0 )
      module = 1;

    int port = wOutput.getport( node );
    int gate = wOutput.getgate( node );
    Boolean dccoutput = StrOp.equals( wOutput.getprot( node ), wOutput.prot_N );
    if( port == 0 )    /* fada used convert to address, port */
      AddrOp.fromFADA( module, &module, &port, &gate );

    long address = (( module - 1 ) * 4 ) + port -1 + (dccoutput?0x3800:0x3000);

    if ( StrOp.equals( wOutput.getcmd( node ), wOutput.on )) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Output %d %s (%s) on", (address - (dccoutput?0x37FF:0x2FFF) ), gate?"b":"a", dccoutput?"dcc":"mm" );
      __setSysMsg(out, 0, CMD_ACC_SWITCH, False, 8, address, gate, 1, swtime/256, swtime%256);
    }
    else {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Output %d %s (%s) off", (address - (dccoutput?0x37FF:0x2FFF) ), gate?"b":"a", dccoutput?"dcc":"mm" );
      __setSysMsg(out, 0, CMD_ACC_SWITCH, False, 8, address, gate, 0, swtime/256, swtime%256);
    }

    ThreadOp.post( data->writer, (obj)out );
    return rsp;
  }

  /* Loco command */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    byte* out = allocMem(32);
    byte* out2 = allocMem(32);
    byte* out3 = allocMem(32);
    Boolean sw     = wLoc.issw( node );
    const char* id = wLoc.getid( node );
    int addr       = wLoc.getaddr( node );
    int fn         = wLoc.isfn( node );
    int addroffset = 0;
    int dir        = 2 - wLoc.isdir( node );
    /* for cs2 1 is forwards, 2 is backwards, from server 1 = forwards, 0 is backwards */
    int speed      = 0;
    int speed1     = 0;
    int speed2     = 0;
    char prot[32]  = {'\0'};

    if( StrOp.equals( wLoc.getprot( node ), wLoc.prot_N ) ) {
      addroffset = 0xC000;
      /* DCC loc adress range start */
      StrOp.copy(prot,"dcc");
    }
    else if( StrOp.equals( wLoc.getprot( node ), wLoc.prot_P ) || StrOp.equals( wLoc.getprot( node ), wLoc.prot_F ) ) {
      addroffset = 0x4000;
      /* MFX loc address range start */
      StrOp.copy(prot,"mfx");
    }
    else {
      addroffset = 0x0000;
      /* MM loc address range start */
      StrOp.copy(prot,"mm");
    }

    long address = addr + addroffset;

    if( wLoc.getV( node ) != -1 ) {
      /* cs2 ranges all speeds from 0 - 1000 regardless of number of steps */
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = wLoc.getV( node ) * 10;
      else if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_step ) ) {
        float fspeed = 1000.0 / (float)wLoc.getspcnt( node );
        fspeed *= wLoc.getV( node );
        speed = (int)fspeed;
      }
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * 1000) / wLoc.getV_max( node );
      speed1 = (speed & 0xFF00) >>8;
      speed2 = speed & 0x00FF;
    }

    if (sw) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco %s %d (%s) %s", id, addr, prot, (dir==1)?"forwards":"backwards" );
      __setSysMsg(out, 0, CMD_LOCO_DIRECTION, False, 5, address, dir, 0, 0, 0);
      __setSysMsg(out2, 0, CMD_LOCO_VELOCITY, False, 6, address, speed1, speed2, 0, 0);
      /* when changing direction cs2 set speed to 0 internally, so after direction change also send speed */
    } else {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco %s %d (%s) speed=%d(%d,%d) %s", id, addr, prot, speed, speed1, speed2, (dir==1)?"forwards":"backwards");
      __setSysMsg(out2, 0, CMD_LOCO_VELOCITY, False, 6, address, speed1, speed2, 0, 0);
      /* also send direction to prevent going wrong way when user has changed direction on the cs2 */
      __setSysMsg(out, 0, CMD_LOCO_DIRECTION, False, 5, address, dir, 0, 0, 0);
    }
    /* send direction first, speed as second, because of CS2 setting speed to 0 on dir change */
    ThreadOp.post( data->writer, (obj)out );
    ThreadOp.post( data->writer, (obj)out2 );
    __setSysMsg(out3, 0, CMD_LOCO_FUNCTION , False, 6, address, 0, fn, 0, 0);
    ThreadOp.post( data->writer, (obj)out3 );
    return rsp;
  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    byte* out = allocMem(32);
    int fnchanged  = wFunCmd.getfnchanged(node);
    const char* id = wFunCmd.getid( node );
    int addr       = wFunCmd.getaddr( node );
    int addroffset = 0;
    char prot[32]  = {'\0'};

    if( StrOp.equals( wLoc.getprot( node ), wLoc.prot_N ) ) {
      addroffset = 0xC000;
      /* DCC loc adress range start */
      StrOp.copy(prot,"dcc");
    }
    else if( StrOp.equals( wLoc.getprot( node ), wLoc.prot_P ) || StrOp.equals( wLoc.getprot( node ), wLoc.prot_F ) ) {
      addroffset = 0x4000;
      /* MFX loc address range start */
      StrOp.copy(prot,"mfx");
    }
    else {
      addroffset = 0x0000;
      /* MM loc address range start */
      StrOp.copy(prot,"mm");
    }
    long address = addr + addroffset;

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "loco %s %d (%s) function f%d to %s", id, addr, prot, fnchanged, __getFunctionState(node, fnchanged)?"on":"off" );

    if( fnchanged != -1 ) {
      __setSysMsg(out, 0, CMD_LOCO_FUNCTION , False, 6, address, fnchanged, __getFunctionState(node, fnchanged), 0, 0);
      ThreadOp.post( data->writer, (obj)out );
      return rsp;
    }
  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    Boolean pom = wProgram.ispom(node);
    int cv = wProgram.getcv( node );
    int value = wProgram.getvalue( node );
    int addr = wProgram.getaddr( node );
    int addroffset = 0;

    if( wProgram.getidentifier(node) != NULL && StrOp.len(wProgram.getidentifier(node))  > 0 ) {
      addr = atoi(wProgram.getidentifier(node));
    }

    if( StrOp.equals( wProgram.getstrval1( node ), wLoc.prot_N ) ) {
      addroffset = 0xC000;
      /* DCC loc adress range start */
    }
    else if( StrOp.equals( wProgram.getstrval1( node ), wLoc.prot_P ) ) {
      addroffset = 0x4000;
      /* MFX loc address range start */
    }
    addr += addroffset;

    if( wProgram.getcmd( node ) == wProgram.get ) {
      byte* out = allocMem(32);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "get CV%d on %s for loco %d...", cv, pom?"POM":"PT", addr );
      __setSysMsg(out, 0, CMD_LOCO_READ_CONFIG , False, 7, addr, cv/256, cv%256, 1, 0);
      ThreadOp.post( data->writer, (obj)out );
    }
    else if( wProgram.getcmd( node ) == wProgram.set ) {
      byte* out = allocMem(32);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set CV%d to %d on %s for loco %d...", cv, value, pom?"POM":"PT", addr );
      __setSysMsg(out, 0, CMD_LOCO_WRITE_CONFIG , False, 8, addr, cv/256, cv%256, value, pom?0x80:0x00);
      ThreadOp.post( data->writer, (obj)out );
    }
  }

  /* Sensor command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFeedback.name() ) ) {
    int addr = wFeedback.getaddr( node );
    Boolean state = wFeedback.isstate( node );

    if( wFeedback.isactivelow(node) )
      wFeedback.setstate( node, !state);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "simulate fb addr=%d state=%s", addr, state?"true":"false" );
    data->listenerFun( data->listenerObj, (iONode)NodeOp.base.clone( node ), TRCLEVEL_INFO );

  }

  return rsp;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOMCS2Data data = Data(inst);
  iONode rsp = NULL;
  if( cmd != NULL ) {
    if( StrOp.equals( NodeOp.getName( cmd ), wLocList.name() ) ) {
      data->locolist = cmd;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "locolist received" );
    }
    else {
      rsp = __translate( (iOMCS2)inst, cmd );
      /* Cleanup Node1 */
      cmd->base.del(cmd);
    }
  }

  return rsp;
}


static void _halt( obj inst, Boolean poweroff, Boolean shutdown ) {
  iOMCS2Data data = Data(inst);
/* semaphore to stop the channelreporter thread */
  data->rprtchnrun = False;
  ThreadOp.sleep(500);
  data->run = False;

  if( data->conOK && wDigInt.isasciiprotocol( data->ini ) && data->initASCII ) {
    ThreadOp.sleep(100);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init ASCII: (C) Close CAN channel" );
    SerialOp.write( data->serial, "C\r", 2 );
  }

  /* wait for all threads... */
  ThreadOp.sleep(500);

  return;
}


static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOMCS2Data data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


static Boolean _setRawListener(obj inst, obj listenerObj, const digint_rawlistener listenerFun ) {
  return True;
}


static byte* _cmdRaw( obj inst, const byte* cmd ) {
  return NULL;
}


/** bit0=power, bit1=programming, bit2=connection */
static int _state( obj inst ) {
  return 0;
}


/* external shortcut event */
static void _shortcut(obj inst) {
  iOMCS2Data data = Data( inst );
}


/**  */
static Boolean _supportPT( obj inst ) {
  return False;
}


static void __genHash() {
  srand (time(NULL));
  rrUID = rand() % 0x8000 + 0x2000;
  rrHash = rrUID & 0xFF7F;
  rrHash |= 0x0300;
}


static void __feedbackMCS2Reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMCS2 mcs2 = (iOMCS2)ThreadOp.getParm( th );
  iOMCS2Data data = Data(mcs2);

  ThreadOp.sleep( 1000 );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2 feedbackpoll started, polling %d S88 units", data->fbmod );
  data->sensor = True;
  do {
      ThreadOp.sleep( 250 );
    __SoD(mcs2);

    if( wDigInt.getprotver( data->ini ) >= 2 ) {
      /* Just poll once for Start of Day. */
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Just poll once for Start of Day. V2" );
      break;
    }

  } while( data->run && (wDigInt.getprotver( data->ini ) == 1 && data->fbmod > 0) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "feedbackpoll ended." );
}


static void __evaluateMCS2S88( iOMCS2Data mcs2, byte* in, unsigned char* prev ) {
  int s88base = in[9] * 16;
  int n = 0;
  int addr = 0;
  int state = 0;
  int t = 0;
  for( t = 0; t < 2; t++) {
    for( n = 0; n < 8; n++ ) {
      addr = s88base + n + 1 + (t * 8);
      state = (in[11 - t] & (0x01 << n)) ? 1:0;
      /* cs2 uses big endian, in11 contains lower 8 inputs, in10 the higher 8 */
      if( prev[addr - 1] != state ) {
        /* this feedback changed state since previous poll */
        prev[addr - 1] = state;
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "fb %d = %d", addr, state );
        {
          /* inform listener: Node */
          iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
          wFeedback.setaddr( nodeC, addr );
          wFeedback.setstate( nodeC, state?True:False );
          if( mcs2->iid != NULL )
            wFeedback.setiid( nodeC, mcs2->iid );
          mcs2->listenerFun( mcs2->listenerObj, nodeC, TRCLEVEL_INFO );
        }
      }
    }
  }

  if(!mcs2->sensor) {
    mcs2->sensor = True;
    __reportState(mcs2);
  }
}


/* 00 23 67 78 08 00 00 00 0B 00 01 00 00 */
static void __evaluateSensorEvent( iOMCS2Data mcs2, byte* in ) {
  int bus    = in[5] * 256 + in[6];
  int addr   = in[7] * 256 + in[8];
  int state = in[10];
  iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "sensor %d:%d state=%d", bus, addr, state );
  wFeedback.setbus( nodeC, bus );
  wFeedback.setaddr( nodeC, addr );
  wFeedback.setstate( nodeC, state?True:False );
  if( mcs2->iid != NULL )
    wFeedback.setiid( nodeC, mcs2->iid );
  mcs2->listenerFun( mcs2->listenerObj, nodeC, TRCLEVEL_INFO );

  if(!mcs2->sensor) {
    mcs2->sensor = True;
    __reportState(mcs2);
  }

}

static int  __getThrottleNr( iOMCS2Data mcs2, byte* in) {
  int hash  = (in[2] << 8) + in[3];
  int nr = 0;
  while( mcs2->ms2UID > 0 && nr < mcs2->ms2UID ) {
    if( hash == ms2hash[nr] ) {
      nr++;
      return(nr);
    }
    nr++;
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MS2 throttle not found! requested: 0x%4X", hash);
  for( nr = 0; nr < mcs2->ms2UID ; nr++ ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Stored MS2 throttle: %d, hash 0x%4x", nr, ms2hash[nr]);
  }
  return(0);
}

static void __evaluateMCS2Loc( iOMCS2Data mcs2, byte* in ) {
  int addr  = ( ( in[7] & 0x0F ) << 8 ) + in[8];
  /* mask left nibble of high byte because this is protocol dependent 0x0 for MM, 0x4 for MFX, 0xC for DCC */
  int speed = (in[9] << 8) + in[10];
  int dir   = in[9];
  int nr = 0;
  char ms2name[] = "MS2-XXX";
  iONode nodeC = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  if( mcs2->iid != NULL )
    wLoc.setiid( nodeC, mcs2->iid );
  wLoc.setaddr( nodeC, addr );
  nr = __getThrottleNr( mcs2, in);
  if( nr > 0 ) {
    if( mcs2->ms2UID == 1 )
      StrOp.copy( ms2name, "MS2");
    else if( mcs2->ms2UID > 1 )
      StrOp.fmtb( ms2name, "MS2-%d", nr);
  }
  wLoc.setthrottleid( nodeC, ( mcs2->mcs2guiUID ? "CS2" : ms2name ) );
  if( in[1] == 0x0A ) {
    /* loc command was a direction command.
       The cs2 sets loc speed to 0 internally on dir change and does not broadcast this. Since the command was not
       send by Rocrail, we have to assume that speed of loc is now set to zero. */
    wLoc.setV_raw( nodeC, 0 );
    wLoc.setV_rawMax( nodeC, 1000 );
    wLoc.setdir( nodeC, dir==1 );
    /* 1 means forwards, 2 means reverse in cs2 message, in Rocrail true=forward, false=reverse */
    /* wLoc.setcmd( nodeC, wLoc.velocity ); */
    wLoc.setcmd( nodeC, wLoc.direction );
  } else {
    /* loc message was speed message */
    wLoc.setV_raw( nodeC, speed );
    wLoc.setV_rawMax( nodeC, 1000 );
    /* all cs2 speeds are on a scale of 0-1000, regardless of actual locdecoder speedsteps */
    wLoc.setcmd( nodeC, wLoc.velocity );
  }
  mcs2->listenerFun( mcs2->listenerObj, nodeC, TRCLEVEL_INFO );
}


static void __evaluateMCS2Function( iOMCS2Data mcs2, byte* in ) {
  int addr     = 0;
  int function = in[9];
  int state    = in[10];
  int nr = 0;
  char ms2name[] = "MS2-XXX";
  addr = ( ( in[7] & 0x0F ) << 8 ) + in[8];
  /* mask left nibble of high byte because this is protocol dependent 0x0 for MM, 0x4 for MFX, 0xC for DCC */
  if( function <= 28 ) {
    /* cs2 supports 32 functions, Rocrail 28 */
    iONode nodeC = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
    if( mcs2->iid != NULL )
      wLoc.setiid( nodeC, mcs2->iid );
    wFunCmd.setaddr( nodeC, addr);
    nr = __getThrottleNr( mcs2, in);
    if( nr > 0 ) {
      if( mcs2->ms2UID == 1 )
        StrOp.copy( ms2name, "MS2");
      else if( mcs2->ms2UID > 1 )
        StrOp.fmtb( ms2name, "MS2-%d", nr);
    }
    wLoc.setthrottleid( nodeC, ( mcs2->mcs2guiUID ? "CS2" : ms2name ) );
    wFunCmd.setfnchanged( nodeC, function);
    wLoc.setcmd( nodeC, wLoc.function );
    switch ( function ) {
      case 0 : wFunCmd.setf0( nodeC, ( state ? True : False)); break;
      case 1 : wFunCmd.setf1( nodeC, ( state ? True : False)); break;
      case 2 : wFunCmd.setf2( nodeC, ( state ? True : False)); break;
      case 3 : wFunCmd.setf3( nodeC, ( state ? True : False)); break;
      case 4 : wFunCmd.setf4( nodeC, ( state ? True : False)); break;
      case 5 : wFunCmd.setf5( nodeC, ( state ? True : False)); break;
      case 6 : wFunCmd.setf6( nodeC, ( state ? True : False)); break;
      case 7 : wFunCmd.setf7( nodeC, ( state ? True : False)); break;
      case 8 : wFunCmd.setf8( nodeC, ( state ? True : False)); break;
      case 9 : wFunCmd.setf9( nodeC, ( state ? True : False)); break;
      case 10 : wFunCmd.setf10( nodeC, ( state ? True : False)); break;
      case 11 : wFunCmd.setf11( nodeC, ( state ? True : False)); break;
      case 12 : wFunCmd.setf12( nodeC, ( state ? True : False)); break;
      case 13 : wFunCmd.setf13( nodeC, ( state ? True : False)); break;
      case 14 : wFunCmd.setf14( nodeC, ( state ? True : False)); break;
      case 15 : wFunCmd.setf15( nodeC, ( state ? True : False)); break;
      case 16 : wFunCmd.setf16( nodeC, ( state ? True : False)); break;
      case 17 : wFunCmd.setf17( nodeC, ( state ? True : False)); break;
      case 18 : wFunCmd.setf18( nodeC, ( state ? True : False)); break;
      case 19 : wFunCmd.setf19( nodeC, ( state ? True : False)); break;
      case 20 : wFunCmd.setf20( nodeC, ( state ? True : False)); break;
      case 21 : wFunCmd.setf21( nodeC, ( state ? True : False)); break;
      case 22 : wFunCmd.setf22( nodeC, ( state ? True : False)); break;
      case 23 : wFunCmd.setf23( nodeC, ( state ? True : False)); break;
      case 24 : wFunCmd.setf24( nodeC, ( state ? True : False)); break;
      case 25 : wFunCmd.setf25( nodeC, ( state ? True : False)); break;
      case 26 : wFunCmd.setf26( nodeC, ( state ? True : False)); break;
      case 27 : wFunCmd.setf27( nodeC, ( state ? True : False)); break;
      case 28 : wFunCmd.setf28( nodeC, ( state ? True : False)); break;
    }
    mcs2->listenerFun( mcs2->listenerObj, nodeC, TRCLEVEL_INFO );
  }
}


static void __reportState(iOMCS2Data data) {
  if( wDigInt.isreportstate(data->ini) && data->listenerFun != NULL && data->listenerObj != NULL ) {
    iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );

    if( data->iid != NULL )
      wState.setiid( node, data->iid );

    wState.setpower( node, data->power );
    wState.settrackbus( node, data->power );
    wState.setsensorbus( node, data->sensor );
    wState.setaccessorybus( node, True );
    wState.setload( node, data->load );
    wState.setvolt( node, data->volt );
    wState.settemp( node, data->temp );
    wState.setshortcut(node, data->overload);
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
}

static void __reportSystemInfo(iOMCS2Data data) {
  if( wDigInt.issysteminfo(data->ini) && data->listenerFun != NULL && data->listenerObj != NULL ) {
    iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );

    if( data->iid != NULL )
      wState.setiid( node, data->iid );

    wState.setload( node, data->load );
    wState.setvolt( node, data->volt );
    wState.settemp( node, data->temp );
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
}


static void __setSwitchtime( iOMCS2Data data ) {
  byte* out = allocMem(32);
  __setSysMsg(out, 0, CMD_SYSTEM, False, 7, 0, CMD_SYSSUB_SWTIME, (data->swtime/10)/256, (data->swtime/10)%256, 0);
  ThreadOp.post( data->writer, (obj)out );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Switch command time set to: %dms", data->swtime );
}


static void __evaluateMCS2System( iOMCS2Data data, byte* in ) {
  int cmd = in[9];
  int addr4 = in[8];
  int addr3 = in[7];
  int addr2 = in[6];
  int addr1 = in[5];
  int dlc   = in[4];
  float tVolt = 0.0;
  float tTemp = 0.0;
  int uid = (in[5] << 24) + (in[6] << 16) + (in[7] << 8) + in[8];
  int gfpUID = data->gbUID ? data->gbUID : data->mcs2gfpUID;
  int nr = 0;
  char ms2name[32] = "\0";
  if( (cmd > 0x0C) && !((cmd == 0x30) || (cmd == 0x80)) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "INVALID system command received: CAN-ID=0x%02X len=%d cmd=0x%02X", in[1]&0xFF, in[4]&0x0F, cmd );
    TraceOp.dump( NULL, TRCLEVEL_EXCEPTION, (char*)in, 13 );
    return;
  }
  if( (dlc == 5 && addr1 == 0 && addr2 == 0 && addr3 == 0 && addr4 == 0) || (dlc == 5 && uid == gfpUID) ) {
    if( in[1] == 0 )
        nr = __getThrottleNr( data, in);
    if( nr == 0 && in[1] == 1)
        StrOp.copy( ms2name, "Gleisbox ACK");
    else if( data->ms2UID == 1 )
        StrOp.copy( ms2name, "MS2");
    else if ( data->ms2UID > 1 )
        StrOp.fmtb( ms2name, "MS2-%d", nr);
    else
        StrOp.copy( ms2name, "MS2-XXX");
    if (cmd == CMD_SYSSUB_STOP && data->power ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "STOP from %s", ( data->gbUID ? ms2name : "CS2" ) );
      data->power = False;
    }
    else if (cmd == CMD_SYSSUB_GO && !data->power ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "GO from %s", ( data->gbUID ? ms2name : "CS2" ) );
      data->power = True;
      data->overload = False;
    }
    __reportState(data);
  }
  if( cmd == CMD_SYSSUB_OVERLOAD ) {
    data->overload = True;
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Overload on UID: 0x%08X on interface: %s", uid, data->iid );
    __reportState(data);
  }
  if( cmd == CMD_SYSSUB_STATUS && in[4] == 8 && wDigInt.issysteminfo(data->ini) ) {
    switch (in[10]){
    case 1:
      /* Current in mA */
      if( data->gbUID != 0 ) 
          data->load = (int)((in[11] * 256 + in[12]) * (1000./824.));
      else
          data->load = (int)((in[11] * 100 + in[12]) * data->loadStepRate);
      if( firstview )
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Current I=0x%04X converted to %dmA for UID: 0x%08X", (in[11] * 256 + in[12]), data->load, uid );
    break;
    case 3:
      /* Voltage in mV */
      if( data->gbUID != 0 ) {
        tVolt = (int)( ( (in[11] * 256 + in[12]) * (1000./185.) ) + 10000);
      }
      else {
        tVolt = ( (in[11] * 256 + in[12]) * data->voltStepRate ) + 10000;
      }
      tVolt = tVolt / 100;
      data->volt = (int)tVolt;
      if( (int)((tVolt-data->volt)*10) >= 5)
          ++data->volt; /* round up */
      data->volt *= 100;
      if( firstview  ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Voltage U=0x%04X converted to %5.0fmV, rounded to %dmV, for UID: 0x%08X", (in[11] * 256 + in[12]), tVolt * 100, data->volt, uid );
      }
    break;
    case 4:
      /* Temperature in degree C */
      if( data->gbUID != 0 ) { 
        tTemp = (in[11] * 256 + in[12]) * (74./202.);
        data->temp = (int)tTemp;
      }
      else {
        tTemp = (in[11] * 256 + in[12]) * data->tempStepRate;
        data->temp = (int)tTemp;
      }
      if( (int)( (tTemp-data->temp) * 10) >= 5 )
          ++data->temp;
      if( firstview ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Temperature t=0x%04X converted to %2.2f, rounded to %d, degree C for UID: 0x%08X", (in[11] * 256 + in[12]), tTemp, data->temp, uid );
      }
    break;
    }
    __reportSystemInfo(data);
  }
}


static void __evaluateMCS2PingCmd( iOMCS2Data data, byte* in ) {
  if( data->mcs2guiUID == 0 && data->ms2UID == 0 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "User interface detected, ping send to identify" );
    __sendMCS2PingCmd(data);
  }
}

static int __calculateGfpHash( iOMCS2Data data, byte* in ) {
  int ctrlhash =  ((in[6] ^ in[8]) << 8) + (in[5] ^ in[7]);
  char devtype[] = "Gleisbox";
  if( (in[11] + in[12]) == 0x00 ) {
    StrOp.copy( devtype, "CS2" );
  }
  ctrlhash &= 0xFF7F;
  ctrlhash |= 0x0300;
  if( in[1] == 0x37 && in[11] == 0 && in[12] == 0x10 ) {
    ctrlhash = ((in[5] ^ in[7]) << 8) + (in[6] ^ in[8]);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CAN Bootloader Service hash calculation requested" );
  }
  int cmd = (in[1] | 1);
  if( cmd == 1 && in[9] == 0x0B) {
    StrOp.copy( devtype, data->gbUID ? "Gleisbox" : "CS2");
  }  
  if( !(cmd == 1 && in[9] == 0x0B) || ctrlhash != ((in[2] << 8) + in[3]) ) 
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "calculated hash for %s 0x%04X", devtype, ctrlhash );
  return(ctrlhash);
}

static int __calculateGuiHash( byte* in ) {
  int ctrlhash = ((in[5] ^ in[7]) << 8) + (in[6] ^ in[8]);
  ctrlhash &= 0xFF80;
  ctrlhash = (ctrlhash << 3);
  ctrlhash |= (in[6] ^ in[8]);
  ctrlhash &= 0xFF7F;
  ctrlhash |= 0x0300;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "calculated hash gui 0x%04X", ctrlhash );
  return(ctrlhash);
}

static int __enumerateMS2( iOMCS2Data data, int dscvrUID) {
  int i, j, flag = 1;
  int tmpUID, tmphash, nr = 0;
  for( i = 1; (i <= data->ms2UID) && flag; i++ ) {
    flag = 0;
    for( j=0; j < data->ms2UID; j++) {
      if( ms2UIDs[j+1] < ms2UIDs[j] ) {
        tmpUID = ms2UIDs[j];
        tmphash = ms2hash[j];
        ms2UIDs[j] = ms2UIDs[j+1];
        ms2hash[j] = ms2hash[j+1];
        ms2UIDs[j+1] = tmpUID;
        ms2hash[j+1] = tmphash;
        flag = 1;
      }
      if( ms2UIDs[j] == dscvrUID ) {
        nr = j;
      }
      else if( ms2UIDs[j+1] == dscvrUID ) {
        nr = j+1;
      }
    }
  }
  return(nr);
}

static void __evaluateMCS2Ping( iOMCS2Data data, byte* in ) {
/* 00000000: 00 31 23 09 08 47 43 4E 61 01 27 00 10 */
  int cstype = in[11] * 256 + in[12];
  int rcvuid = (in[5] << 24) + (in[6] << 16) + (in[7] << 8) + in[8];
  int rcvhash = (in[2] << 8) + in[3];
  int ctrlhash;
  int nr = 0;
  cstype &= 0xFFF0;
  if( in[1] == ( CAN_ID_PING | BIT_RESPONSE ) ) {
    switch( cstype )
    {
      case 0x0010:
        if( data->gbUID != rcvuid ) {
            if( data->gbUID == 0 && data->mcs2gfpUID == 0 ) {
              data->gbUID = rcvuid;
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Ping type: 0x%04X -> Gleisbox UID: 0x%08X stored for hash: 0x%04X", cstype, data->gbUID, rcvhash );
            }
            else {
              TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: Two Gleisbox systems found on CAN bus interface %s, Gleisbox 0x%08X cleared", data->iid, rcvuid );
            }
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Gleisbox firmware version = %d.%d", in[9], in[10] );
        }
        break;
      case 0x0000: 
        if( data->mcs2gfpUID != rcvuid ) {
            if(  data->gbUID == 0 && data->mcs2gfpUID == 0 ) {
              data->mcs2gfpUID = rcvuid;
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Ping type: 0x%04X -> CS2-GFP UID: 0x%08X stored for hash: 0x%04X", cstype, data->mcs2gfpUID, rcvhash );
            }
            else {
              TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: Two GFP systems found on CAN bus interface %s", data->iid );
              if( data->gbUID != 0 && data->mcs2gfpUID == 0 ) {
                data->mcs2gfpUID = rcvuid;
                TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Gleisbox with UID 0x%08X cleared", data->gbUID );
                data->gbUID = 0;
              }
              else {
                TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "CS2 with UID 0x%08X ignored", rcvuid );
              }
            }
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CS2-GFP firmware version = %d.%d", in[9], in[10] );
        }
        break;
      case 0xFFF0:
        ctrlhash = __calculateGuiHash( in );
        if( data->mcs2guiUID != rcvuid ) { 
/* the CS2 GUI type responses with the CAN hash of the PING sender.... */
          if( rcvhash == ctrlhash || (rcvhash == rrHash && ((rcvuid - 1) == data->mcs2gfpUID)) ) {
            data->mcs2guiUID = rcvuid;
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Ping type: 0x%04X -> CS2-GUI UID: 0x%08X stored", cstype, data->mcs2guiUID );
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CS2-GUI firmware version = %d.%d", in[9], in[10] );
          }
          else {
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Invalid Ping packet received for type CS2 GUI UID 0x%08X", rcvuid );           
          }
        }
        break;
    /* despite the CAN documentation the "Geraetekennung" of a MS2 is 0x0032 */
      case 0x0030:
        ctrlhash = __calculateGuiHash( in );
          if( rcvhash == ctrlhash ) {
            int dscvrUID = (in[5] << 24) + (in[6] << 16) + (in[7] << 8) + in[8];
            while( data->ms2UID > 0 && nr < data->ms2UID ) { 
              if( dscvrUID == ms2UIDs[nr] && ms2hash[nr] == rcvhash ) {
                return;
              }
              if( dscvrUID == ms2UIDs[nr] ) {
                TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Discovered type: 0x%04X -> MS-2 UID: 0x%08X with other hash: 0x%04X, new hash: 0x%04X", cstype, dscvrUID, ms2hash[nr], rcvhash );
                break;
              }
              nr++;
            }
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Writing in array at position %d", nr );
            ms2UIDs[nr] = dscvrUID;
            ms2hash[nr] = rcvhash; 
            nr = __enumerateMS2(data, dscvrUID);
            data->ms2UID++;
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Ping type: 0x%04X -> MS-2 UID: 0x%08X stored. %d MS2 device(s) detected", cstype, ms2UIDs[nr], data->ms2UID );
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MS-2 firmware version = %d.%d", in[9], in[10] );
          }
        break;
      default: 
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Unknown Ping for type: 0x%04X", cstype );
    } 
  }
/* the CS2 GUI type responses with the CAN hash of the PING sender.... */
  if( rcvhash == ctrlhash && rcvhash == rrHash && cstype != 0xFFF0) { 
    __genHash();      
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Duplicate CANbus address detect for hash: 0x%04X, new hash generated: 0x%04X", rcvhash, rrHash );
  }

  /*  if requested announce ourself as a CANbus member, nice but for now it only generates troubles */
/*  byte  buffer[32];
  buffer[0]  = (rrUID  & 0xFF000000) >> 24;
  buffer[1]  = (rrUID  & 0x00FF0000) >> 16;
  buffer[2]  = (rrUID  & 0x0000FF00) >> 8;
  buffer[3]  = (rrUID  & 0x000000FF);
  buffer[4]  = (revisionnr / 100) & 0xFF;
  buffer[5]  = (revisionnr % 100) & 0xFF;
  buffer[6]  = (sysgerken / 256) & 0xFF;
  buffer[7]  = (sysgerken % 256) & 0xFF;
  ThreadOp.post( data->writer, (obj)__makeMsg(0, CAN_CMD_PING, True, 8, buffer) );
*/
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "mcs2gfpUID: 0x%08X  mcs2guiUID: 0x%08X ms2UID: 0x%08X gbUID: 0x%08X on interface %s", data->mcs2gfpUID, data->mcs2guiUID, ms2UIDs[nr], data->gbUID, data->iid );

}

static void __evaluateMCS2Switch( iOMCS2Data mcs2, byte* in ) {
  int addr1 = in[7] & 0x0F;
  /* mask left nibble of high byte, this is not part of the actual address (always 0x3 for accessory) */
  int addr2 = in[8];
  int state = in[9];
  int port  = 0;
  int addr  = 0;

  if( addr1 > 7 )
    addr1 = addr1 - 8;
    /* address range start 0x3000 for MM, range start for DCC is 0x3800, so MM 0x3000 or DCC 0x3800 is Rocrail address 1, port 1 */
  addr2 = addr2 + (addr1 << 8) + 1;
  AddrOp.fromPADA( addr2, &addr, &port );

  iONode nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
  if( mcs2->iid != NULL )
    wSwitch.setiid( nodeC, mcs2->iid );
  wSwitch.setaddr1( nodeC, ( addr ) );
  wSwitch.setport1( nodeC, ( port ) );
  wSwitch.setstate( nodeC, (state == 1)?"straight":"turnout" );
  mcs2->listenerFun( mcs2->listenerObj, nodeC, TRCLEVEL_INFO );
}


static Boolean __findLocinList(iOMCS2Data data) {
  if( data->locolist != NULL ) {
    iONode loProps = wLocList.getlc( data->locolist );

    while ( loProps != NULL ) {
      const char *loName = wLoc.getid( loProps );

      byte addrFormat = 0x00;
/*      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "comparing %s in locolist name: %s, registered sid: %d, requested sid: %d", data->id, loName, wLoc.getaddr(loProps), data->sid); */
      
      if( StrOp.equals( wLoc.prot_F, wLoc.getprot( loProps ) )  || StrOp.equals( wLoc.prot_P, wLoc.getprot( loProps ) ) ) {
        /* mfx */
        addrFormat = 0x40;
      }
      if( (wLoc.getaddr(loProps) == data->sid) && (addrFormat == 0x40) ) {
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "mfx discovered loc already in list, sid: %d, name: %s, iid: %s, preventing duplicate.", data->sid, loName, data->iid);
        return True;
      }
      
      if( StrOp.equals( data->id, loName ) ) {
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "loc id already in list, for name: %s, registered sid: %d, requested sid: %d, iid: %s", loName, wLoc.getaddr(loProps), data->sid, data->iid);
        data->sid = wLoc.getaddr(loProps);
        return True;
      }
      loProps = wLocList.nextlc(data->locolist, loProps);
    }
  }
  return False;
}


static Boolean __findSidinLocList(iOMCS2Data data) {
int addr;
  if( data->locolist != NULL ) {
    iONode loProps = wLocList.getlc( data->locolist );

    while ( loProps != NULL ) {
      const char *loName = wLoc.getid( loProps );
      if( wLoc.getaddr(loProps) == data->sid ) {
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "locolist requested sid: %d found, name: %s", wLoc.getaddr(loProps), loName);
        return True;
      }
      loProps = wLocList.nextlc(data->locolist, loProps);
    }
  }
  return False;
}


static Boolean __findSidinProductList(iOMCS2Data data) {
  
  iONode loco = wMCS2.getproduct(data->mcs2ini);

  while( loco != NULL ) {
    if( wProduct.getsid(loco) == data->sid ) {
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "productlist requested sid: %d found, name: %s", wProduct.getsid(loco), wProduct.getdesc(loco));
      return True;
    }
    loco = wMCS2.nextproduct(data->mcs2ini, loco);
  }
  return False;
}


static iONode __getUID(iOMCS2Data data) {
  int sid;
  
  iONode loco = wMCS2.getproduct(data->mcs2ini);

  while( loco != NULL ) {
    if( wProduct.getpid(loco) == data->uid ) {
      return loco;
    }
    loco = wMCS2.nextproduct(data->mcs2ini, loco);
  }
  return NULL;
}


static int __countUID(iOMCS2Data data) {
  int cnt = 0;
  iONode loco = wMCS2.getproduct(data->mcs2ini);

  while( loco != NULL ) {
    cnt++;
    loco = wMCS2.nextproduct(data->mcs2ini, loco);
  }
  return cnt;
}


static iONode __getBySID(iOMCS2Data data, int sid) {
  iONode loco = wMCS2.getproduct(data->mcs2ini);

  while( loco != NULL ) {
    if( wProduct.getsid(loco) == sid ) {
      return loco;
    }
    loco = wMCS2.nextproduct(data->mcs2ini, loco);
  }
  return NULL;
}


static byte __getnewSID(iOMCS2Data data) {
  if( data->sid < 4 )
    data->sid = 4;
  while( __findSidinLocList(data) || __findSidinProductList(data) ) {
    data->sid++;
  }
  TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "new sid: %d", data->sid);
  return data->sid;
}


static void __clearRegMfxVar(iOMCS2Data data) {
  int i;
  if( data->sid != 0 ) 
      TraceOp.trc(name, TRCLEVEL_MONITOR, __LINE__, 9999, "mfx registration for loco: %s, sid: %d ended.", idname, data->sid);
  data->sid = 0;
  data->uid = 0;
  data->xS2handling = 0;
  data->mfxDetectInProgress = False;
  for( i=0; i<17 ; i++ ) {
    idname[i] = '\0';
  }
}


static Boolean __checkForValidName(iOMCS2Data data) {
  int i;
  char adaptname[17];
  TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "checking for valid name: %s", idname);
  if( StrOp.len(idname) > 0 ) {
    for( i=0 ; i < StrOp.len(idname) ; i++ ) {
      if( idname[i] < ' '  || idname[i] > '~' || i == 32) {
        StrOp.fmtb( adaptname, "mfx-%d", data->sid);
        StrOp.copy( idname, adaptname );
        TraceOp.trc(name, TRCLEVEL_ERROR, __LINE__, 9999, "Invalid name, adapting loconame to: %s", idname);
        return False; 
      } 
    }
  }
  else {
    StrOp.fmtb( adaptname, "mfx-%d", data->sid);
    StrOp.copy( idname, adaptname );
    TraceOp.trc(name, TRCLEVEL_ERROR, __LINE__, 9999, "zero length, adapted loconame: %s, data->id: %s", idname, data->id);
    return False;
  }
  return True;
}

static Boolean __checkForDuplicateNameId(iOMCS2Data data, Boolean prevent) {
  char sidnr[5] =  "\0";
  iONode loProps;
  char*  adaptname   = allocMem(32);
  if( data->locolist != NULL ) {
    loProps = wLocList.getlc( data->locolist );
    while ( loProps != NULL ) {
      const char *loName = wLoc.getid( loProps );
      if( loName != NULL ) {
        if( StrOp.equals( data->id, loName ) && data->sid != wLoc.getaddr(loProps) ) {
          if( prevent ) {
            StrOp.fmtb( sidnr, " (%d)", data->sid);
            StrOp.copy( adaptname, idname);
            adaptname = StrOp.cat( adaptname, sidnr);
            StrOp.copy( idname, adaptname);
            TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "discovered loco %s has dupicate id in locolist, new loco renamed to: %s", loName, data->id);
          }
          return True;
        }
      }
      loProps = wLocList.nextlc(data->locolist, loProps);
    }
  }

  loProps = wMCS2.getproduct(data->mcs2ini);
  while( loProps != NULL ) {
    const char *loName =wProduct.getdesc(loProps);
    if( loName != NULL ) {
      if( StrOp.equals( data->id, loName ) && data->sid != wProduct.getsid(loProps) ) {
        if( prevent ) {
          StrOp.fmtb( sidnr, " (%d)", data->sid);
          StrOp.copy( adaptname, idname);
          adaptname = StrOp.cat( adaptname, sidnr);
          StrOp.copy( idname, adaptname);
          TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "discovered loco %s has dupicate id in productlist, new loco renamed to: %s", loName, data->id);
        }
        return True;
      }
    }
    loProps = wMCS2.nextproduct(data->mcs2ini, loProps);
  }

  return False;
}

static void __reqFullName(iOMCS2Data data, byte* in) {
  byte buffer[32];
  buffer[0] = 0;
  buffer[1] = 0;
  buffer[2] = in[9] | 0x40; 
  buffer[3] = in[10];
  buffer[4] = 0x04; /* index 1 */
  buffer[5] = 0x03; /* cv3 */
  buffer[6] = 0x10;
  ThreadOp.post(data->writer, (obj)__makeMsg(0, CMD_LOCO_READ_CONFIG, False, 7, buffer));
  TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Requesting full name for UID=0x%08X, address %d", data->uid, in[10]);
}

static void __registerMCS2DetectedMfxLoco(iOMCS2Data data) {
  char identifier[32] = {'\0'};
  int i;
  iONode loco = __getUID(data);
  if( data->sid == 1 ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "ERROR; Tried to register mfx loc with sid=1" );
    Boolean modifyname = False;
    if( StrOp.equals( wProduct.getdesc(loco), "mfx-1") )
        modifyname = True;
    wProduct.setsid(loco, __getnewSID( data));
    if( modifyname ) {
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Wiping invalid product name mfx-1");
      for( i=0; i<7 ; i++ ) {
        idname[i] = '\0';
      }
      __checkForValidName(data);
      wProduct.setdesc(loco, data->id);
    }
    TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "product sid: %d, UID: 0x%08X, id: %s registrated",  wProduct.getsid(loco), data->uid, data->id);
    if( (data->ms2UID != 0 || data->mcs2guiUID != 0) ) {
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Loc creation failed due to invalid sid, error in %cS2 registration controlled procedure",  ( data->ms2UID ? 'M' : 'C' ) );
      __clearRegMfxVar(data);
      return;
    }
  }
/* check here for unique name id */
  if( __checkForDuplicateNameId(data, True) )
      wProduct.setdesc(loco, data->id);
  if( !__findLocinList(data) ) {
    StrOp.copy( idname, wProduct.getdesc(loco) );
    TraceOp.trc(name, TRCLEVEL_MONITOR, __LINE__, 9999, "Registering request id: %s, sid: %d, UID: 0x%08X,  controllerpath: %s", data->id, data->sid, data->uid, data->iid);
    if( loco == NULL ) {
      loco = NodeOp.inst(wProduct.name(), data->mcs2ini, ELEMENT_NODE);
      NodeOp.addChild(data->mcs2ini, loco);
      wProduct.setpid(loco, data->uid);
      wProduct.setdesc(loco, data->id);
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "product added sid: %d, UID: 0x%08X, id: %s",  wProduct.getsid(loco), data->uid, data->id);
    }
    wProduct.setsid(loco, data->sid);
    TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "product marked sid: %d, UID: 0x%08X, id: %s registrated",  wProduct.getsid(loco), data->uid, data->id);
    wProduct.setcid(loco,1);

    iONode lc = NodeOp.inst(wLoc.name(), NULL, ELEMENT_NODE);
    wLoc.setaddr(lc, wProduct.getsid(loco));
    StrOp.fmtb(identifier, "%u", data->uid);
    wLoc.setidentifier(lc, identifier);
    wLoc.setid(lc, data->id);
    wLoc.setprot(lc, wLoc.prot_F); /* MFX */
    wLoc.setcmd(lc, wLoc.discover);
    if (data->iid != NULL)
      wLoc.setiid(lc, data->iid);
    data->listenerFun(data->listenerObj, lc, TRCLEVEL_INFO);
  }
  else {
    TraceOp.trc(name, TRCLEVEL_MONITOR, __LINE__, 9999, "ID: %s, sid: %d,  UID: %08X found in locolist, marked as registered", data->id, data->sid, data->uid);
    wProduct.setcid(loco,1);
  }
  __clearRegMfxVar(data);
}


static void __evaluateMCS2ReadConfig(iOMCS2Data data, byte* in) {
  int index =  (in[9] & 0xFC) >> 2;
  int cv    =  (in[9] & 0x03) * 256 + in[10];
  int addr  =  ((in[7] & 0x0F) << 8) + in[8];
  int value =  in[11];
  int i     =  0;
  int dlc   =  in[4];
  iONode loco;

  if( dlc == 7 ) {
    if( cv != 3 || (cv == 3 && value < 0x20) ) {
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "sid: %d, index: %d, cv: %d, value: 0x%02X", addr, index, cv, value);
    }
    else {
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "sid: %d, index: %d, cv: %d, value: 0x%02X,-> '%c'", addr, index, cv, value, value);
    }
    if( cv == 3 ) {
      if( index == 1 && StrOp.len(idname) > 0 ) {
        data->mfxDetectInProgress = True;
        for( i = 0; i < 17 ; i++ ) {
          idname[i] = '\0';
        }
      }
      idname[index - 1] = in[11];
      if( ((index == 0x10 || in[11] == 0) && idname[0] != '\0') || (idname[0] == '\0' && index == 1) ) {
        data->sid = addr;
        loco = __getBySID(data, addr);
        wProduct.setdesc(loco, data->id);
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "catched sid: %d, product sid: %d, UID: 0x%08X, name: %s, iid: %s", addr, wProduct.getsid(loco), wProduct.getpid(loco), data->id, data->iid);
  	/* in case of a xS2 controlled procedure, the sid adress changed from 1 to x in the end of the registering procedure */ 
        if( addr > 1 ) {   
          if( !__checkForValidName(data) ) {
            wProduct.setdesc(loco, data->id);
          }
          TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "register from readconfig name sid: %d, UID: 0x%08X, name: %s, iid: %s", addr, wProduct.getpid(loco), data->id, data->iid);
          data->uid = wProduct.getpid(loco);
          __registerMCS2DetectedMfxLoco(data);
        }
        if( data->ms2UID == 0 && data->mcs2guiUID == 0 && (wProduct.getsid(loco) == 1 || data->sid == 1) ) {
          if( __findLocinList(data)) {
            wProduct.setsid(loco, data->sid);
          } 
          else {
            wProduct.setsid(loco, __getnewSID( data));
          }
          byte  buffer[32];
          buffer[0]  = (wProduct.getpid(loco) & 0xFF000000) >> 24;;
          buffer[1]  = (wProduct.getpid(loco) & 0x00FF0000) >> 16;
          buffer[2]  = (wProduct.getpid(loco) & 0x0000FF00) >> 8;
          buffer[3]  = (wProduct.getpid(loco) & 0x000000FF);
          buffer[4]  = (wProduct.getsid(loco) / 256) & 0xFF;
          buffer[5]  = (wProduct.getsid(loco) % 256) & 0xFF;
          ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_LOCO_BIND, False, 6, buffer) );
          TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "bind posted for UID: 0x%08X, sid: %d", wProduct.getpid(loco), wProduct.getsid(loco) );
          idname[0] = '\0';
        }
      }
    }
    
    iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );

    cv = in[9]*256 + in[10];
    wProgram.setcv( node, cv );
    wProgram.setvalue( node, value );
    wProgram.setcmd( node, wProgram.datarsp );
    if( data->iid != NULL )
      wProgram.setiid( node, data->iid );

    if( data->listenerFun != NULL && data->listenerObj != NULL )
      data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );

  }
}


static void __evaluateMCS2WriteConfig( iOMCS2Data data, byte* in ) {
  int dlc = in[4];
  int index =  (in[9] & 0xFC) >> 2;
  int cv    =  (in[9] & 0x03) * 256 + in[10];
  int addr  =  ((in[7] & 0x0F) << 8) + in[8];
  int value =  in[11];
  int i     =  0;
  if( dlc == 8 ) {
    if( cv != 3 || (cv == 3 && value < 0x20) ) {
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "sid: %d, index: %d, cv: %d, value: 0x%02X", addr, index, cv, value);
    }
    else {
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "sid: %d, index: %d, cv: %d, value: 0x%02X,-> '%c'", addr, index, cv, value, value);
    }
    if( cv == 3 && index == 1 && StrOp.len(idname) > 0 ) {
      data->mfxDetectInProgress = True;
      for( i = 0; i < 17 ; i++ ) {
        idname[i] = '\0';
      }
    }
    if( index != 0 && cv == 3 ) {
      idname[index - 1] = value;
    }
    if( cv == 3 && value == 0 ) {
      iONode loco = __getBySID(data, addr);
      data->uid = wProduct.getpid(loco);
      data->sid = wProduct.getsid(loco);
      wProduct.setdesc(loco, data->id);
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "sid: %d, UID: 0x%08X, loc %s, iid: %s", data->sid, data->uid, data->id, data->iid);
      TraceOp.trc(name, TRCLEVEL_MONITOR, __LINE__, 9999, "Name changed: %s, address: %d, UID: 0x%08X, valid at restart", data->id, wProduct.getsid(loco), wProduct.getpid(loco));
      if( __checkForValidName(data) ) {
        __registerMCS2DetectedMfxLoco(data);
      }
      else {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Invalid name, loc not registered in loclist" );
      }
    }
  }
  else {
    iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    cv = in[9]*256 + in[10];
    wProgram.setcv( node, cv );
    wProgram.setvalue( node, value );
    wProgram.setcmd( node, wProgram.datarsp );
    if( data->iid != NULL )
        wProgram.setiid( node, data->iid );
    if( data->listenerFun != NULL && data->listenerObj != NULL )
        data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
}

static void __evaluateMCS2Verify( iOMCS2Data mcs2, byte* in ) {
/* 00064711 6 FF FA 8C 43 00 05 */
  mcs2->sid  = (in[9] << 8) + in[10];
  mcs2->uid =  (in[5] << 24) + (in[6] << 16) + (in[7] << 8) + in[8];
/* in case of a CS2 we verify also at a bind, a direct bind generates an error in the CS2 GUI */
  if( wMCS2.isdiscovery(mcs2->mcs2ini)  || (wMCS2.isbind(mcs2->mcs2ini) && mcs2->mcs2guiUID != 0) || (mcs2->ms2UID == 0 && mcs2->mcs2guiUID == 0) ) {
    if( mcs2->sid > 0 ) {
      
      iONode loco = __getUID(mcs2);
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Bind verified succesfull for UID=0x%08X to address %d", mcs2->uid, mcs2->sid);

      if( mcs2->ms2UID == 0 && mcs2->mcs2guiUID == 0 && mcs2->sid == 1 && wProduct.getcid(loco) == 0 ) {
        __reqFullName(mcs2, in);
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "full name requested for sid: %d", mcs2->sid);
      }
      /* sid 1 is used during registering */
      if( (mcs2->sid > 1)  && wProduct.getcid(loco) == 0 ){
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Registering by verify UID=0x%08X, address %d", mcs2->uid, mcs2->sid);
        StrOp.copy( idname, wProduct.getdesc(loco) );            
        if( !__checkForValidName(mcs2) && mcs2->gbUID != 0 ) {
          __reqFullName(mcs2, in);
        }
        else {
          wProduct.setdesc(loco, mcs2->id);
          __registerMCS2DetectedMfxLoco(mcs2);
        }
      }
      else {
        if( wProduct.getcid(loco) == 1 ){
          if( mcs2->sid > 1 ) {
            TraceOp.trc(name, TRCLEVEL_MONITOR, __LINE__, 9999, "ID: %s, sid: %d,  UID: %08X already handled or created", wProduct.getdesc(loco), mcs2->sid, mcs2->uid);
          }
          else {
            TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "ID: %s, sid: %d,  UID: %08X already handled or created", wProduct.getdesc(loco), mcs2->sid, mcs2->uid);
          }
        }
        if( mcs2->sid > 1 )
            __clearRegMfxVar(mcs2);
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Verify failed for UID=0x%08X; loco %s", mcs2->uid , mcs2->id );
      __clearRegMfxVar(mcs2);
    }
  }
  else {
    TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Verify for UID=0x%08X discarded (Verify is not activated by Discovery option)", mcs2->uid);
  }
}


static void __evaluateMCS2Bind( iOMCS2Data mcs2, byte* in ) {
  mcs2->uid = (in[5] << 24) + (in[6] << 16) + (in[7] << 8) + in[8];
  mcs2->sid = (in[9] << 8) + in[10];
  
  if ( wMCS2.isbind(mcs2->mcs2ini) || wMCS2.isdiscovery(mcs2->mcs2ini) ) {
    TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "mfx bind invoked for UID=0x%08X, for sid: %d", mcs2->uid, mcs2->sid);
    mcs2->mfxDetectInProgress = True;
    iONode loco = __getUID(mcs2);
    if( loco == NULL ) {
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Bind UID=0x%08X creating product, assigneded to address %d", mcs2->uid, mcs2->sid);
      loco = NodeOp.inst(wProduct.name(), mcs2->mcs2ini, ELEMENT_NODE);
      NodeOp.addChild(mcs2->mcs2ini, loco);
      wProduct.setpid(loco, mcs2->uid);
      wProduct.setsid(loco, mcs2->sid);
    }  
    else {
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Bind UID=0x%08X (%s) product already assigned to address %d", wProduct.getpid(loco), wProduct.getdesc(loco), wProduct.getsid(loco));
      if( wProduct.getdesc(loco) != NULL ) { 
        StrOp.copy( idname, wProduct.getdesc(loco) );
      }
    }       

    if( wProduct.getsid(loco) != mcs2->sid ) {
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Bind UID=0x%08X modifying changed sid %d to new sid %d", wProduct.getpid(loco), wProduct.getsid(loco), mcs2->sid);
      wProduct.setsid(loco, mcs2->sid);
    }
    if( wMCS2.isdiscovery(mcs2->mcs2ini) || (mcs2->ms2UID != 0 && wMCS2.isdiscovery(mcs2->mcs2ini)) || mcs2->mcs2guiUID != 0 ) {    
      if( (mcs2->ms2UID == 0) && (mcs2->mcs2guiUID == 0) ) {
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Bind acknowledged, requesting Verify UID=0x%08X on sid: %d", mcs2->uid, mcs2->sid);
        byte  buffer[32];
        buffer[0]  = in[5];
        buffer[1]  = in[6];
        buffer[2]  = in[7];
        buffer[3]  = in[8];
        buffer[4] = (mcs2->sid / 256) & 0xFF;
        buffer[5] = (mcs2->sid % 256) & 0xFF;
        ThreadOp.post(mcs2->writer, (obj)__makeMsg(0, CMD_LOCO_VERIFY, False, 6, buffer));
      }
      else {
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Bind listening (no action) for CS2/MS2 controlled procedure. UID: 0x%08X on sid: %d", mcs2->uid, mcs2->sid);
      }
    }
    else {
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Bind only enabled, ending Bind for UID=0x%08X (discovery is not activated)", mcs2->uid);
      if( !__findSidinLocList(mcs2) ) {
        if( __checkForValidName(mcs2) ) {
          iONode loco = __getBySID(mcs2, mcs2->sid);
          wProduct.setdesc(loco, mcs2->id);
          __registerMCS2DetectedMfxLoco(mcs2);
        }
        else {
/* we have a new product, however discover is disabled, deal with it if no GUI detected*/
	  if( mcs2->ms2UID == 0 && mcs2->mcs2guiUID == 0 && mcs2->sid == 1) {
            byte  buffer[32];
            buffer[0]  = in[5];
            buffer[1]  = in[6];
            buffer[2]  = in[7];
            buffer[3]  = in[8];
            buffer[4] = (mcs2->sid / 256) & 0xFF;
            buffer[5] = (mcs2->sid % 256) & 0xFF;
            ThreadOp.post(mcs2->writer, (obj)__makeMsg(0, CMD_LOCO_VERIFY, False, 6, buffer));
            TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "New product UID=0x%08X found, no GUI detected, continuing registering procedure", mcs2->uid);
	  }
	  else {
            TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Product UID=0x%08X not added due to invalid (incomplete) name", mcs2->uid);
            __clearRegMfxVar(mcs2);
          }
        }
      }
      else {
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Product UID=0x%08X already in locolist, not added", mcs2->uid);
        __clearRegMfxVar(mcs2);
      }
    }
  }
  else {
    TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Bind for UID=0x%08X discarded (bind & discovery is not activated)", mcs2->uid);
  }
}


static void __evaluateMCS2Discovery( iOMCS2Data mcs2, byte* in ) {
/*
  T000303005FFE405E920<\r>
*/
  int dlc = in[4];
  if( dlc == 0 ){
    mcs2->mfxDetectInProgress = False;
    TraceOp.trc(name, TRCLEVEL_MONITOR, __LINE__, 9999, "Discovery completed, ready for operation");
  }

  if( !mcs2->mfxDetectInProgress && dlc == 6 && in[1] == 0x03 && (in[5] & in[6] & in[7] & in[8] == 0xFF) ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Start mfx detect by gfp software detected" );
    mcs2->mfxDetectInProgress = True;
  }

  if( dlc == 5 && mcs2->uid != (in[5] << 24) + (in[6] << 16) + (in[7] << 8) + in[8] ) {
    mcs2->uid = (in[5] << 24) + (in[6] << 16) + (in[7] << 8) + in[8];
    int protocoltype = in[9];
    TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Discovery requested for UID=0x%08X", mcs2->uid);
    if( wMCS2.isdiscovery(mcs2->mcs2ini) || mcs2->mcs2gfpUID != 0 || mcs2->gbUID != 0 ) {
      mcs2->mfxDetectInProgress = True;
      iONode loco = __getUID(mcs2);
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Registration status for UID=0x%08X (%s) is %s", mcs2->uid, wProduct.getdesc(loco), wProduct.getcid(loco) ? "registered" : "not registered");
      if ( loco != NULL ) {
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Discovery UID=0x%08X (%s) already assigned to address %d", mcs2->uid, wProduct.getdesc(loco), wProduct.getsid(loco));
        mcs2->sid = wProduct.getsid(loco);
        if( wProduct.getdesc(loco) != NULL ) { 
          StrOp.copy( idname, wProduct.getdesc(loco) );
        }
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Discovery UID=0x%08X name in structure: %s", wProduct.getpid(loco), mcs2->id);
      }
      if( mcs2->ms2UID == 0 && mcs2->mcs2gfpUID == 0) {
        if( loco == NULL || StrOp.len(idname) == 0 ) {
          if( loco == NULL ) {
            loco = NodeOp.inst(wProduct.name(), mcs2->mcs2ini, ELEMENT_NODE);
            NodeOp.addChild(mcs2->mcs2ini, loco);
            wProduct.setpid(loco, mcs2->uid);
          }
          mcs2->sid = 1;
          TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Discovery UID=0x%08X to temporary address %d", mcs2->uid, mcs2->sid);
          wProduct.setsid(loco, 1);
        }
        byte  buffer[32];
        buffer[0]  = in[5];
        buffer[1]  = in[6];
        buffer[2]  = in[7];
        buffer[3]  = in[8];
        buffer[4]  = (mcs2->sid / 256) & 0xFF;
        buffer[5]  = (mcs2->sid % 256) & 0xFF;

        ThreadOp.post( mcs2->writer, (obj)__makeMsg(0, CMD_LOCO_BIND, False, 6, buffer) );
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Bind requested by Rocrail for sid %d", mcs2->sid);
      }
      else {
            TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Discovered UID=0x%08X handled by CS2/MS2", mcs2->uid);
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Discovery discarded for UID=0x%08X (discovery is not activated)", mcs2->uid );
    }
  }
}

static void __evaluateMCS2CANBootBound( iOMCS2Data data, byte* in ) {
  int cstype = in[11] * 256 + in[12];
  int highword = 0;
  int lowword =0;
  int controlhash =0;
  int rcvHash =0;
  cstype &= 0xFFF0;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Evaluate CAN boot bound for type: 0x%04X", cstype );
  if( cstype == 0x0010 && data->ms2UID == 0 && data->mcs2gfpUID == 0 && data->gbUID == 0 ) {
        data->gbUID = (in[5] << 24) + (in[6] << 16) + (in[7] << 8) + in[8];
        byte  buffer[32];
        buffer[0]  = 0;
        buffer[1]  = 0;
        buffer[2]  = 0;
        buffer[3]  = 0;
        buffer[4]  = 0x11;
        buffer[5]  = 0;
        buffer[6]  = 0;
        buffer[7]  = 0;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Sending boot with current version command to standalone Gleisbox with UID: 0x%08X", data->gbUID );
        ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_CAN_BOOT_BOUND, False, 5, buffer) );
        ThreadOp.sleep(2000);
        buffer[4]  = 0;
        buffer[1]  = (rrUID  & 0xFF000000) >> 24;
        buffer[2]  = (rrUID  & 0x00FF0000) >> 16;
        buffer[3]  = (rrUID  & 0x0000FF00) >> 8;
        buffer[4]  = (rrUID  & 0x000000FF);
        ThreadOp.post( data->writer, (obj)__makeMsg(0, CAN_CMD_PING, False, 0, buffer) );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CAN boot not handled, unknown type: 0x%04X", cstype );
  }
}

static byte __cnlExponent() {
  devconfig[1] ^= 0xFF;
  devconfig[1]++;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "For channel %d, the exponent value is: %d", devconfig[0], devconfig[1] );
  return(devconfig[1]);
}

static int __cnlZeroRange() {
  int zeroRange = 0;
  zeroRange = devconfig[6]*256+devconfig[7];
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Zero value for channel %d is: %d", devconfig[0], zeroRange );
  return(zeroRange);
}

static int __cnlEndRange() {
  int endRange = 0;
  endRange = devconfig[14] * 256 + devconfig[15];
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Converted 0x%4x to %d[10] as maximum value for channel %d", endRange, endRange, devconfig[0] );
  return(endRange);
}

static float __cnlConvertStr2Float( iOMCS2Data data ) {
  char strbuffer[6] = { '\0' }; 
  int i = 0;
  data->idxcnt++;
  while( devconfig[data->idxcnt] != 0 && i < 6 ) {
    strbuffer[i] = devconfig[data->idxcnt];
    data->idxcnt++;
    i++;
  }
  strbuffer[i] = devconfig[data->idxcnt];
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "String %s converted to float: %.2f", strbuffer, atof(strbuffer) );
  return(atof(strbuffer));
}

static float __cnlStepRate(int zero, int end, float start, float full, int exponent) {
  float steprate = 0.0;

  steprate = ( (full - start) * pow( 10.0, devconfig[1])  / (end - zero) );
  steprate = ((int)(steprate * 100000 + .5) / 100000.0);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Channel %d, value %d - %d on scale %.2f - %.2f. Calculated steprate is: %.5f", devconfig[0], zero, end, start, full, steprate );
  return (steprate);
}  
   
static void __evaluateChannel( iOMCS2Data data ) {
  int i = 0;
  int exponent = 0;
  int zeroRange = 0;
  int endRange = 0;
  float startRange = 0.0;
  float fullRange = 0.0;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Evaluating channel: %d", data->lstChn );
  if( data->lstChn == devconfig[0] ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "databuffer meets status data configuration channel config: %d", data->lstChn );
    data->idxcnt = 16; 
    while( data->idxcnt < 48 && devconfig[data->idxcnt] != 0 ) {
      devstrbuffer[i] = devconfig[data->idxcnt];
      data->idxcnt++;
      i++;
    }
    exponent = __cnlExponent();
    zeroRange = __cnlZeroRange();
    endRange = __cnlEndRange();
    startRange = __cnlConvertStr2Float(data);
    fullRange = __cnlConvertStr2Float(data);
    if( StrOp.equals( devstrbuffer, "MAIN" ) ) {
      reportstatechannel[0] = data->lstChn;
      data->loadStepRate = __cnlStepRate(zeroRange, endRange, startRange, fullRange, exponent);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Channel index for current sensing is: %d", data->lstChn );
    }
    else if( StrOp.equals( devstrbuffer, "VOLT" ) ) {
      reportstatechannel[1] = data->lstChn;
      data->voltStepRate = __cnlStepRate(zeroRange, endRange, startRange, fullRange, exponent);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Channel index for tension sensing is: %d", data->lstChn );
    }
    else if( StrOp.equals( devstrbuffer, "TEMP" ) ) {
      reportstatechannel[2] = data->lstChn;
      data->tempStepRate = __cnlStepRate(zeroRange, endRange, startRange, fullRange, exponent);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Channel index for temperature sensing is: %d", data->lstChn );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Channel config buffer for %s is ignored.", devstrbuffer );
    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Error during reading channel config: %d, data in buffer for channel: %d", data->lstChn, devconfig[0] );
  }
}

static void __evaluateMCS2StatusConfig( iOMCS2Data data, byte* in ) {
/* we have to store
last received packet, 0 is not busy

handled Index */
  int i = 0;
  int x = 0;
  char artno[8] = { '\0' };
  if( in[2] == 0x03 )
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Receive config status data for channel: %d, packet no: %d", data->lstChn, in[3]);
  if( in[2] == 0x03 && data->lstChn == 0 ) {
    data->lastseqnr = in[3];
    switch( in[3] ) {
      case 1:
        data->indices = in[5];
        data->configchnl = in[6];
        int serno = (in[9] << 24) + (in[10] << 16) + (in[11] << 8) + in[12];
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "The serialnumber of your device is: %d", serno );
        break;
      case 2:
        for( i=0 ; i < 8 ; i++) {
          artno[i] = in[i+5];
        }
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "The article number of your device is: %s", artno );
        break;
      default:
        if( data->idxcnt < 32 ) {
          Boolean eostr = False;
          for(i = 0; i < in[4]; i++ ) {
            devstrbuffer[data->idxcnt] = in[i+5];
            if( in[i+5] == 0 )
                eostr = True; 
            data->idxcnt++;
          }
          if( eostr ) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "The article name of your device is: %s", devstrbuffer );
            x = StrOp.len(devstrbuffer);
            for( i = 0 ; i < x; i++ ) {
              devstrbuffer[i] = '\0';
            }
          }
        }
        else if ( data->idxcnt >= 32 ) {
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Running out of buffer space while receiving article name");
        }
    }
  }

  if( in[2] == 0x03 && data->lstChn > 0 && data->idxcnt < 48) {
    data->lastseqnr = in[3];
    for(i = 0; i < in[4]; i++ ) {
      devconfig[data->idxcnt] = in[i+5];
      data->idxcnt++;
    }
  }
  else if ( data->idxcnt >= 48 ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Running out of buffer space while receiving config status data");
  }

  int rcvuid =  (in[5] << 24) + (in[6] << 16) + (in[7] << 8) + in[8];
/* the condition (in[10] == 0 || in[10] == data->lastseqnr)  is a fix for the marklin bug omitting the number of send packets in[10] == data->lastseqnr */ 
  if( rcvuid == data->mcs2gfpUID && in[4] == 5 &&  data->lstChn == in[9] && (in[10] == 0 || in[10] == data->lastseqnr) ) {
    if( data->lstChn > 0 )
        __evaluateChannel(data);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "End of status data configuration transfer for channel: %d", data->lstChn );
    data->lastseqnr = 0;  /* ready to interrogate next index */ 
  }
}

static void __evaluateCCSwitch( iOMCS2Data mcs2, byte* in ) {
  int addr1 = in[7] & 0x0F;
  /* mask left nibble of high byte, this is not part of the actual address (always 0x3 for accessory) */
  int addr2 = in[8];
  int state = in[9];
  int port  = 0;
  int addr  = 0;

  if( addr1 > 7 )
    addr1 = addr1 - 8;
    /* address range start 0x3000 for MM, range start for DCC is 0x3800, so MM 0x3000 or DCC 0x3800 is Rocrail address 1, port 1 */
  addr2 = addr2 + (addr1 << 8) + 1;
  AddrOp.fromPADA( addr2, &addr, &port );

  if( state == 0xFF) {
    /* error */
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "CC Switch %d.%d report: ERROR", addr, port );
  }
  else {
    iONode nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
    if( mcs2->iid != NULL )
      wSwitch.setiid( nodeC, mcs2->iid );
    wSwitch.setaddr1( nodeC, ( addr ) );
    wSwitch.setport1( nodeC, ( port ) );
    wSwitch.setstate( nodeC, (state == 0xFE)?"turnout":"straight" );
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "CC Switch %d.%d report: 0x%02X", addr, port, state );
    mcs2->listenerFun( mcs2->listenerObj, nodeC, TRCLEVEL_INFO );
  }
}


static byte __HEXA2Byte( const char* s ) {
  char val[3] = {0};
  val[0] = s[0];
  val[1] = s[1];
  val[2] = '\0';
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "HEXA=[%s]", val );
  return (unsigned char)(strtol( val, NULL, 16)&0xFF);
}


static void __Byte2HEXA( char* s, byte b ) {
  static char cHex[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  int i = 0;
  s[0] = cHex[(b&0xF0)>>4 ];
  s[1] = cHex[ b&0x0F     ];
}


static Boolean __convertASCII2Bin( char* inASCII, byte* in) {
  MemOp.set(in, 0, 32);
  if( inASCII[0] != 'T') {
    StrOp.replaceAll(inASCII, '\r', ' ');
    TraceOp.trc( name, inASCII[0] == 'Z' ? TRCLEVEL_DEBUG:TRCLEVEL_MONITOR, __LINE__, 9999, "ASCII message: %s", inASCII );
    in[1] = 0xFF;
    return False;
  }
  else {
    int i = 0;
    /*
     * Tiiiiiiii l dd..[CR]
     * T0000030050000000001[CR] */
    in[4] = inASCII[9] - '0';
    in[0] = __HEXA2Byte(inASCII+1);
    in[1] = __HEXA2Byte(inASCII+3);
    in[2] = __HEXA2Byte(inASCII+5);
    in[3] = __HEXA2Byte(inASCII+7);

    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "ASCII to bin: len=%d", in[4] );
    for( i = 0; i < in[4]; i++ ) {
     in[5+i] = __HEXA2Byte(inASCII+10+(i*2));
    }

    return True;
  }
  return False;
}


static int __convertBin2ASCII( const byte* out, char* outASCII) {
  int i = 0;
  /*
   * Tiiiiiiii l dd..[CR]
   * T00360301 5 00 00 00 00 11 [CR] */
  outASCII[0] = 'T';
  outASCII[1] = '\0';

  __Byte2HEXA(outASCII+1, out[0]);
  __Byte2HEXA(outASCII+3, out[1]);
  __Byte2HEXA(outASCII+5, out[2]);
  __Byte2HEXA(outASCII+7, out[3]);
  outASCII[9] = out[4] + '0';

  for(i = 0; i < out[4]; i++ ) {
    __Byte2HEXA(outASCII+10+(i*2), out[5+i]);
  }
  outASCII[10+(i*2)] = '\r';
  outASCII[10+(i*2)+1] = '\0';

  return StrOp.len(outASCII);
}

static void __reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMCS2 mcs2 = (iOMCS2)ThreadOp.getParm( th );
  iOMCS2Data data = Data(mcs2);
  byte in[32];
  int mod = 0;
  unsigned char store[1024];
  int retry = 0;
  Boolean hashError = False;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MGBOX reader started." );

  for( mod = 0; mod < 1024; mod++) {
    store[mod] = 0;
    /* storage container for feedback states to check on changes */
  }

  data->id = idname;

  do {
    MemOp.set(in, 0, 32);
    if( data->udp ) {
      if( SocketOp.recvfrom( data->readUDP, (char*)in, 13, NULL, NULL ) <= 0 ) {
        SocketOp.base.del(data->readUDP);
        ThreadOp.sleep(1000);
        if( data->run ) {
          data->readUDP = SocketOp.inst( wDigInt.gethost(data->ini), wDigInt.getudpportRX(data->ini), False, True, False );
          SocketOp.bind(data->readUDP);
          continue;
        }
        else break;
      }
    }
    else {
      if( data->conOK ) {
        /* Init ASCII protocol if needed. */
        if( wDigInt.isasciiprotocol( data->ini ) && !data->initASCII ) {
          Boolean ok = True;
          if(ok) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init ASCII: (S5) 250 kBaud" );
            ok = SerialOp.write( data->serial, "S5\r", 3 );
            ThreadOp.sleep(500); /* sleep baby sleep */
          }
          if(ok) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init ASCII: (O) Open CAN channel." );
            ok = SerialOp.write( data->serial, "O\r", 2 );
            ThreadOp.sleep(500); /* sleep baby sleep */
          }
          if(ok) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init ASCII: (V) Get hardware version." );
            ok = SerialOp.write( data->serial, "V\r", 2 );
            ThreadOp.sleep(500); /* sleep baby sleep */
          }

          if( ok ) {
            byte*  cmd   = allocMem(32);
            MemOp.set(cmd, 0, 32);
            cmd[0] = (0x360301 & 0xFF000000) >> 24;
            cmd[1] = (0x360301 & 0x00FF0000) >> 16;
            cmd[2] = (0x360301 & 0x0000FF00) >> 8;
            cmd[3] = (0x360301 & 0x000000FF);
            cmd[4] = 5;
            cmd[9] = 0x11;
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "send magic ASCII" );
            ThreadOp.post( data->writer, (obj)cmd );
            data->initASCII = True;
          }
        }

        int available = SerialOp.available(data->serial);
        if( available > 0 ) {
          if( wDigInt.isasciiprotocol( data->ini ) ) {
            char inASCII[64] = {'\0'};
            Boolean CR = False;
            int idx = 0;
            while(SerialOp.available(data->serial) > 0 ) {
              if( SerialOp.read( data->serial, (char*)(inASCII+idx), 1 ) ) {
                if( SerialOp.available(data->serial) == 0 )
                  ThreadOp.sleep(10);
                idx++;
                inASCII[idx] = '\0';
                if( inASCII[idx-1] == '\r' ) {
                  CR = True;
                  break;
                }
              }
              else {
                break;
              }
            }
            if( CR ) {
              TraceOp.trc( name, inASCII[0] != 'T' ? TRCLEVEL_DEBUG:TRCLEVEL_BYTE, __LINE__, 9999, "ASCII read: %s", inASCII );
              __convertASCII2Bin(inASCII, in);
            }
          }
          else {
            int idx = 0;
            while(SerialOp.available(data->serial) > 0 && idx < 13) {
              if( SerialOp.read( data->serial, (char*)(in+idx), 1 ) ) {
                if( SerialOp.available(data->serial) == 0 )
                  ThreadOp.sleep(10);
                idx++;
              }
              else {
                break;
              }
            }
            TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "%d bytes read, nr of data bytes is %d", idx, in[4]&0x0F );
          }

        }
        else if( available == -1 || SerialOp.getRc(data->serial) > 0 ) {
          /* device error */
          data->conOK = False;
          SerialOp.close(data->serial);
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "device error" );
          ThreadOp.sleep(10);
          if( data->run ) continue;
          else break;
        }
        else {
          ThreadOp.sleep(10);
          if( data->run ) continue;
          else break;
        }
      }
      /* !data->conOK */
      else {
        ThreadOp.sleep(100);
        retry++;
        if( retry >= 50 ) {
          retry = 0;
          data->conOK = SerialOp.open( data->serial );
          if(data->conOK) {
            SerialOp.flush(data->serial);
            data->initASCII = False;
          }
        }
        if( data->run ) continue;
        else break;
      }
    }
    if( in[1] != 0xFF )
      TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)in, 13 );

    /* CS2 communication consists of commands (command byte always even) and replies. Reply byte is equal to command byte but with
       response bit (lsb) set, so always odd. When Rocrail sends a command, this is not broadcasted by the CS2, only the reply
       is broadcasted. When a command is issued from the CS2 user interface, both the command and the reply is broadcasted.
       This means that when a command (even) is received, Rocrail did not send that command. */
    int ctrlHash;
    int cmd = (in[1] | 1);
    if( cmd == 0x31 && (in[4] > 4) && in[12] > 0x10) {
      ctrlHash = __calculateGuiHash( in );
    }
    else {
      if( (cmd == 1 && in[9] == 0x0B) || (cmd == 0x31 && in[4] > 0) || cmd == 0x37 || cmd == 0x3B ) {
        ctrlHash = __calculateGfpHash( data, in );
      }
    }
    hashError = False;
    if( !( (ctrlHash == (in[2] << 8) + in[3]) || ( (in[2] & 0x03) == 0x03 && (in[3] & 0x80) == 0 ) ) )
        hashError = True;
    if( ((in[0] & 0xF0) > 0) || ((in[1] > 0x45) && (in[1] != 0x60)) || hashError || (in[4] > 8) ) {
      TraceOp.dump( NULL, TRCLEVEL_EXCEPTION, (char*)in, 13 );
      if( (in[0] & 0xF0) > 0 )
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "INVALID packet; PRIO byte contains too high value: prio=0x%02X",  in[0]&0xFF );
      if( ((in[1] > 0x45) && (in[1] != 0x60)) )
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "INVALID packet; command value: CAN-ID=0x%02X", in[1]&0xFF );
      if( hashError )
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "INVALID packet; CAN hash value: 0x%04X", ((in[2] << 8) + in[3]) );
      if( in[4] > 8 )
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "INVALID packet; DLC byte contains too high value (max=8): ", in[4]&0x0F );
    }
    else if( in[1] == ID_SYSTEM || in[1] == (ID_SYSTEM + BIT_RESPONSE) ) {
      /*System command */
      __evaluateMCS2System( data, in );
    }
    else if( in[1] == CAN_S88_REPORT && in[4] == 7 ) {
      /* unoffcial reply to unofficial polling command, don't care if the poll was from Rocrail or not, always good to have the S88 state. */
      __evaluateMCS2S88( data, in, store );
    }
    else if( in[1] == CAN_SENSOR_EVENT ) {
      __evaluateSensorEvent( data, in );
    }
    else if( in[1] == 0x22 && in[4] == 8 ) { /* Without response bit set!? (CAN Digital Bahn) */
      __evaluateSensorEvent( data, in );
    }
    else if( in[1] ==  CAN_ID_PING ) {
      __evaluateMCS2PingCmd( data, in );
    }
     else if( in[1] == ( CAN_ID_PING | BIT_RESPONSE ) ) {
      __evaluateMCS2Ping( data, in );
    }
    else if( in[1] == ID_LOCO_DIRECTION | in[1] == ID_LOCO_VELOCITY ) {
      /* loc speed or direction comamnd, not from Rocrail. */
      __evaluateMCS2Loc( data, in );
    }
    else if( in[1] == ID_LOCO_FUNCTION ) {
      /* locfunction command, not from Rocrail. */
      __evaluateMCS2Function( data, in );
    }
    else if( in[1] == ID_ACC_SWITCH && in[10] == 0x01 ) {
      /* switch command gate activated, second command with gate deactivated again is ignored, not from Rocrail. */
      __evaluateMCS2Switch( data, in );
    }
    else if( in[1] == ID_ACC_SWITCH_RSP && (in[9] & 0x80) ) {
      /* CC-Schnitte */
      __evaluateCCSwitch( data, in );
    }
    else if( in[1] == (ID_LOCO_DISCOVERY + BIT_RESPONSE) ) {
      __evaluateMCS2Discovery( data, in );
    }
    else if( in[1] == (ID_LOCO_BIND + BIT_RESPONSE) ) {
       __evaluateMCS2Bind( data, in );
    }
    else if( in[1] == (ID_LOCO_VERIFY + BIT_RESPONSE) ) {
      __evaluateMCS2Verify( data, in );
    }
    else if( in[1] == (ID_LOCO_READ_CONFIG + BIT_RESPONSE) ) {
      __evaluateMCS2ReadConfig( data, in );
    }
    else if( in[1] == (ID_LOCO_WRITE_CONFIG + BIT_RESPONSE) ) {
      __evaluateMCS2WriteConfig( data, in );
    }
    else if( in[1] == (ID_CAN_BOOT_BOUND + BIT_RESPONSE) ) {
       __evaluateMCS2CANBootBound( data, in );
    }
    else if( in[1] == (ID_SYS_STAT_DATA + BIT_RESPONSE) ) {
       __evaluateMCS2StatusConfig( data, in );
    }
    else if( in[1] != 0xFF ) {
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "Above packet unhandled: CAN-ID=0x%02X len=%d", in[1]&0xFF, in[4]&0x0F );
    }
    ThreadOp.sleep(0);

  } while( data->readerrun );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MGBOX reader stopped on interface: %s", data->iid );
}

static void __binder( iOMCS2Data data) {
  byte  buffer[32];
  int guiTimeout = 0;
  int mfxTimeout = 0;
  int regTimeout = 0;
  byte in[32];

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Bind/Discovery option started." );
  
  iONode loco = wMCS2.getproduct(data->mcs2ini);
  while( loco != NULL ) {
    wProduct.setcid(loco, 0);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Clearing registration for product: %s, product marked as %s", wProduct.getdesc(loco), wProduct.getcid(loco) ? "registered" : "not registered" );
    loco = wMCS2.nextproduct(data->mcs2ini, loco);
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "binder recognized devices:");
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "mcs2gfpUID: 0x%08X  mcs2guiUID: 0x%08X ms2UID: 0x%08X gbUID: 0x%08X", data->mcs2gfpUID, data->mcs2guiUID, ms2UIDs[0], data->gbUID );

  if( data->ms2UID != 0 || data->mcs2gfpUID != 0 || data->gbUID != 0 ) {
    if( data->mcs2gfpUID != 0 )
        ThreadOp.sleep(5500);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Number of locos in product table: %d", __countUID(data) );
    loco = wMCS2.getproduct(data->mcs2ini);
    while( data->run && loco != NULL ) {
      while( data->run && (!data->power || (data->mfxDetectInProgress && (mfxTimeout < 60))) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Waiting for power (%s) or mfx detection in progress (%s)", data->power?"on":"off", data->mfxDetectInProgress?"yes":"no" );
        ThreadOp.sleep(1150);
        if( !data->mfxDetectInProgress )
            ThreadOp.sleep(5500);
        mfxTimeout++;
      }
      mfxTimeout = 0;
      if( (regTimeout % 25) == 0 )
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "product list processing waits for handling UID: %08X", data->uid );
      regTimeout++;
      if( regTimeout == 481 ) {
        data->uid = 0;
      }
      if( data->run && data->uid == 0 ) {
        regTimeout = 0;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Fetching product UID: %08X", wProduct.getpid(loco) );
        if( wProduct.getcid(loco) == 0 ) {
          data->uid = wProduct.getpid(loco);
          data->sid = wProduct.getsid(loco);
          StrOp.copy( idname, wProduct.getdesc(loco) );
          if( idname[0] == 0) {
            TraceOp.trc( name, TRCLEVEL_ERROR, __LINE__, 9999, "No id in rocrail.ini for UID: %08X, sid: %d", wProduct.getpid(loco), wProduct.getsid(loco) );
          }
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Binder: registered status for loco %s is %s", idname, wProduct.getcid(loco) ? "registered" : "not registered" );
          if( (data->mcs2gfpUID != 0 || data->gbUID != 0) && wProduct.getcid(loco) == 0 ) {
            if( __findLocinList(data) && __checkForValidName(data) ) {
              wProduct.setcid(loco, 1);
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Binder found product in locolist, loco %s product marked as registered", wProduct.getdesc(loco) );
            }
            buffer[0]  = (data->uid & 0xFF000000) >> 24;
            buffer[1]  = (data->uid & 0x00FF0000) >> 16;
            buffer[2]  = (data->uid & 0x0000FF00) >> 8;
            buffer[3]  = (data->uid & 0x000000FF);
            if( (data->ms2UID == 0 && data->mcs2guiUID == 0 && data->mcs2gfpUID == 0 && wMCS2.isbind(data->mcs2ini)) ||
                (data->ms2UID != 0 && data->mcs2guiUID == 0 && data->mcs2gfpUID == 0 && wMCS2.isbind(data->mcs2ini) && !wMCS2.isdiscovery(data->mcs2ini)) ) {
              buffer[4]  = (wProduct.getsid(loco) / 256) & 0xFF;
              buffer[5]  = (wProduct.getsid(loco) % 256) & 0xFF;
              TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "bind requested for %s, pid 0x%08X, sid %d", data->id, data->uid, data->sid);
              ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_LOCO_BIND, False, 6, buffer) );
            }
            else {
              if( wMCS2.isbind(data->mcs2ini) && wMCS2.isdiscovery(data->mcs2ini) && data->mcs2gfpUID == 0 ) {
                TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "discover requested for %s, pid 0x%08X, sid %d", data->id, data->uid, data->sid);
                buffer[4]  = 0x20;
                buffer[5]  = 0;
                ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_LOCO_DISCOVERY, False, 5, buffer) );
              }
/* A CS2 GUI accepts a verify for a loco without marking it as found new (in this case the mfx logo does not appear in the display) */
              guiTimeout = 0;
              while( data->mcs2guiUID == 0 && data->ms2UID == 0 && guiTimeout < 12) {
                TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Product 0x%08X %s, sid %d waiting for the gui device.", data->uid, data->id, data->sid);
                ThreadOp.sleep(5000);
                guiTimeout++;
              }
              if( (wMCS2.isdiscovery(data->mcs2ini)  && !wMCS2.isbind(data->mcs2ini)) || data->mcs2gfpUID != 0 ) {
                TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Verify requested UID=0x%08X on sid: %d", data->uid, data->sid);
                buffer[4] = (data->sid / 256) & 0xFF;
                buffer[5] = (data->sid % 256) & 0xFF;
                ThreadOp.post(data->writer, (obj)__makeMsg(0, CMD_LOCO_VERIFY, False, 6, buffer));
              }
            }
          }
        }
        else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Product UID: %08X already registered %s SID %d", wProduct.getpid(loco), wProduct.getdesc(loco), wProduct.getsid(loco));
        }
        loco = wMCS2.nextproduct(data->mcs2ini, loco);
        if( loco != NULL ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Pointer advanced to next product");
        }
        else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "End of productlist reached.");
        }
      }
      if( loco != NULL && wProduct.getpid(loco) == 0 ) {
          loco = wMCS2.nextproduct(data->mcs2ini, loco);
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Check your rocrail.ini it contains a corrupt row with a product UID: 0 under the controller section.");
      } 
      if( loco != NULL ) {
        ThreadOp.sleep(125);
      }
    }
  }
  /* enable the desired dcc protocol for configured loco */\
  if( data->locolist != NULL ) {
    iONode loProps = wLocList.getlc( data->locolist );
    char prot[32]  = {'\0'};
    int addr;

    while ( loProps != NULL ) {
      if( StrOp.equals( wLoc.getprot( loProps ), wLoc.prot_N ) || StrOp.equals( wLoc.getprot( loProps ), wLoc.prot_N ) ) {
        const char *loName = wLoc.getid( loProps );
        byte  buffer[32];
        buffer[0]  = 0x00;
        buffer[1]  = 0x00;
        buffer[2]  = (wLoc.getaddr(loProps) / 256) & 0xFF;
        buffer[2] |= 0xC0;
        buffer[3]  = (wLoc.getaddr(loProps) % 256) & 0xFF;
        buffer[4]  = CMD_SYSSUB_LOCOPROT;
        if( StrOp.equals( wLoc.getprot( loProps ), wLoc.prot_N ) ) {
          switch(  wLoc.getspcnt( loProps ) ) {
            case 14:
              buffer[5] = LOCO_PROT_DCC_14;
              StrOp.copy(prot,"DCC-14");
            break;
            case 126:
            case 128:
              buffer[5] = LOCO_PROT_DCC_128;
              StrOp.copy(prot,"DCC-128");
            break;
            default:
              buffer[5] = LOCO_PROT_DCC_28;
              StrOp.copy(prot,"DCC-28 (default)");
            break;
          }
        }
        if( StrOp.equals( wLoc.getprot( loProps ), wLoc.prot_L ) ) {
          if( wLoc.getspcnt( loProps ) > 28) { 
            buffer[5] = LOCO_PROT_DCC_L_128;
            StrOp.copy(prot,"DCC-128 long address");
          }
          else {
            buffer[5] = LOCO_PROT_DCC_L_28;
            StrOp.copy(prot,"DCC-28 long address");
          }
        }
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "locolist sid: %d, name: %s configured for %s using %d steps.", wLoc.getaddr(loProps), loName, prot, wLoc.getspcnt( loProps ));
        ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_SYSTEM, False, 6, buffer) );
      }
      loProps = wLocList.nextlc(data->locolist, loProps);
    }
  }
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Thread for mfx bind completed on interface: %s.", data->iid  );
}


static void __xS2Svc( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMCS2 mcs2 = (iOMCS2)ThreadOp.getParm( th );
  iOMCS2Data data = Data(mcs2);
  int i = 0;
  byte buffer[32];
  int GFP;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CAN device detection started on interface: %s.", data->iid );
  if( !data->power ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Request CAN configuration by ping" );
    __sendMCS2PingCmd(data);
  }
  ThreadOp.sleep(150);  /* give time to ping to determine the CAN bus config */
/* Wait until at least one CANbus member is detected */
  if( data->ms2UID == 0 && data->gbUID == 0 && data->mcs2guiUID == 0 && data->mcs2gfpUID == 0 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "No CS2/MS2 CAN devices detected on iid: %s, sending boot to standalone Gleisbox", data->iid );
    ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_CAN_BOOT_BOUND, False, 0, buffer) );
    ThreadOp.sleep(2040);
  }

  while( data->run && i < 8 && data->ms2UID == 0 && data->mcs2guiUID == 0 && data->mcs2gfpUID == 0){
    if( data->gbUID != 0 ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Start configuring Gleisbox");
      buffer[4]  = CMD_SYSSUB_NEWREGNR;
      if( wMCS2.isdiscovery(data->mcs2ini) ) {
        buffer[6]  = rand() % 23;
        if( buffer[6]  < 4 ) {
          buffer[6]  += 4;
        }
      }
      else {
        buffer[6] = __getnewSID(data);
      }
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Set mfx sid counter to %d.", buffer[6] );
      ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_SYSTEM, False, 7, buffer) );
      buffer[4]  = CMD_SYSSUB_ENAPROT;
      buffer[5]  = PROT_MM2 | PROT_MFX | PROT_DCC;
      buffer[6]  = 0;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Enabling protocols: MM2, MFX & DCC");
      ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_SYSTEM, False, 6, buffer) );
      if( data->power) {
        buffer[4]  = CMD_SYSSUB_GO;
        buffer[5]  = 0;
        ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_SYSTEM, False, 5, buffer) );
      }      
      ThreadOp.sleep(2500);
      break;
    }
    if( i == 7 ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "CANbus device error, no MS2 / Gleisbox detected on iid: %s", data->iid );
      break;
    }
    ThreadOp.sleep(100);
    i++;
  }

  __setSwitchtime(data);
  if( wMCS2.isbind(data->mcs2ini) || wMCS2.isdiscovery(data->mcs2ini) ) {
    __binder(data);
  }
  ThreadOp.sleep(2050);
  while( data->uid != 0 || data->mfxDetectInProgress ) {
    ThreadOp.sleep(1000);
  }
            
  if( wMCS2.isdiscovery(data->mcs2ini) ) {
    buffer[0] = 0;
    buffer[4] = 0;
    TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "mfx full discovery requested.");
    ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_LOCO_DISCOVERY, False, 1, buffer) );
  }

  if( wDigInt.issysteminfo(data->ini) ) {
    ThreadOp.sleep(500);
    while( data->mfxDetectInProgress ) {
      ThreadOp.sleep(250);
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Channel reportstate started on interface: %s.", data->iid );
    if( data->gbUID != 0 )
        GFP = data->gbUID;
    else
        GFP = data->mcs2gfpUID;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "UID for GFP reportstate: 0x%08X", GFP );
/* ask  for channel report config on CS2 */
    if( data->mcs2gfpUID != 0 ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CS2 gfp: ox%08X Channel Reporter configuration requested on interface: %s.", data->mcs2gfpUID, data->iid );
      byte buffer[32];
      data->lstChn = 0;

      while( data->run && data->mcs2gfpUID != 0 && data->lstChn <= data->indices  ) {
        buffer[0]  = (data->mcs2gfpUID & 0xFF000000) >> 24;
        buffer[1]  = (data->mcs2gfpUID & 0x00FF0000) >> 16;
        buffer[2]  = (data->mcs2gfpUID & 0x0000FF00) >> 8;
        buffer[3]  = (data->mcs2gfpUID & 0x000000FF);
        buffer[4] = data->lstChn;
        data->idxcnt = 0;
        ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_SYS_STAT_DATA, False, 5, buffer) );
        ThreadOp.sleep (30);
        i = 0;
        while( data->lastseqnr != 0 || i == 20) {
          ThreadOp.sleep (20);
          i++;
        }
        data->lstChn++;
      }
    }

/* ask  for channel status for load, tension and temperature*/
    i = 0;
    while( data->rprtchnrun && (data->gbUID != 0 || data->mcs2gfpUID != 0) ) {
      byte buffer[32];
      buffer[0]  = (GFP & 0xFF000000) >> 24;
      buffer[1]  = (GFP & 0x00FF0000) >> 16;
      buffer[2]  = (GFP & 0x0000FF00) >> 8;
      buffer[3]  = (GFP & 0x000000FF);
      buffer[4] = CMD_SYSSUB_STATUS;
      buffer[5] = reportstatechannel[i]; /* 1= current 3= voltage 4= temp */
      if( !data->mfxDetectInProgress )
          ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_SYSTEM, False, 6, buffer) );
      i++;
      if( firstview )
        ThreadOp.sleep (50);
      else
        ThreadOp.sleep (500);
      if( i == 3 ) {
        i = 0;
        firstview = False;
      }
    }
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Boot helper and CS2/Gleisbox Channel Reporter ended on interface: %s.", data->iid );
  
}

static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMCS2 mcs2 = (iOMCS2)ThreadOp.getParm( th );
  iOMCS2Data data = Data(mcs2);

  if(wDigInt.isasciiprotocol( data->ini )) {
    while( !data->initASCII && data->run ) {
      ThreadOp.sleep(10);
    }
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MGBOX writer started with hash: 0x%04X", rrHash );

  do {
    byte* cmd = (byte*)ThreadOp.getPost( th );
    if (cmd != NULL) {
      if( data->udp ) {
        TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)cmd, 13 );
        SocketOp.sendto( data->writeUDP, (char*)cmd, 13, NULL, 0 );
      }
      else {
        if( wDigInt.isasciiprotocol( data->ini ) ) {
          char out[64] = {'\0'};
          int len = __convertBin2ASCII(cmd, out);
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "ASCII write: %s", out );
          SerialOp.write( data->serial, out, len );
        }
        else {
          TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)cmd, 13 );
          SerialOp.write( data->serial, (char*)cmd, 13 );
        }
      }

      freeMem( cmd );
    }

    ThreadOp.sleep(data->cmdpause);
  } while( data->run );
/* trigger the reader thread */
  if( data->udp ) {
    byte* out = allocMem(32);
    __setSysMsg(out, 0, CMD_SYSTEM, False, 5, 0, CMD_SYSSUB_STOP, 0, 0, 0);
    TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, 13 );
    SocketOp.sendto( data->writeUDP, (char*)out, 13, NULL, 0 );
    freeMem( out );
  }

  data->readerrun = False;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MGBOX writer stopped on interface: %s", data->iid );
}



/* VERSION: */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOMCS2Data data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


/**
 * UDP Broadcast address = wDigInt.gethost()
 * UDP send port 15730 (fix)
 * UDP recv port 15731 (fix)
 *
 * */
static struct OMCS2* _inst( const iONode ini ,const iOTrace trc ) {
  iOMCS2 __MCS2 = allocMem( sizeof( struct OMCS2 ) );
  iOMCS2Data data = allocMem( sizeof( struct OMCS2Data ) );
  MemOp.basecpy( __MCS2, &MCS2Op, 0, sizeof( struct OMCS2 ), data );
  int portTX = 15731;
  int portRX = 15730;
  char procnr[2];
  char procname[17];
  srand (time(NULL));

  /* Initialize data->xxx members... */
  TraceOp.set( trc );
  
  rrUID = ('R' << 24) + ('c' << 16) + ('r' << 8) + 'L';
  rrHash = ( 'R' ^ 'r') + (('c' ^ 'L') << 8);
  rrHash &= 0xFF7F;
  rrHash |= 0x0300;  

  data->ini = ini;
  data->mcs2ini = wDigInt.getmcs2(ini);
  if( data->mcs2ini == NULL ) {
    data->mcs2ini = NodeOp.inst( wMCS2.name(), NULL, ELEMENT_NODE );
    NodeOp.addChild( ini, data->mcs2ini);
  }
  data->cmdpause = wMCS2.getcmdpause(data->mcs2ini);
  data->sensorbegin = wMCS2.getsensorbegin(data->mcs2ini);
  data->sensorend   = wMCS2.getsensorend(data->mcs2ini);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MGBOX %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  s88 modules      [%d]", wDigInt.getfbmod( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  version          [%d]", wDigInt.getprotver( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  sensor device ID [%d]", wMCS2.getfbdevid(data->mcs2ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  command pause    [%d]ms", data->cmdpause );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  sensor range     [%d-%d]", data->sensorbegin, data->sensorend );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  hash             [0x%04X]", rrHash );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  system info      [%s]", wDigInt.issysteminfo(data->mcs2ini)?"yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  discovery        [%s]", wMCS2.isdiscovery(data->mcs2ini)?"yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  bind             [%s]", wMCS2.isbind(data->mcs2ini)?"yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  report state     [%s]", wDigInt.isreportstate(data->ini)?"yes":"no" );

  data->udp = !StrOp.equals( wDigInt.sublib_serial, wDigInt.getsublib(data->ini));

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  sublib           [%s]", wDigInt.getsublib(data->ini) );
  if( data->udp ) {
    if( wDigInt.getudpportTX(data->ini) > 0 )
      portTX = wDigInt.getudpportTX(data->ini);
    if( wDigInt.getudpportRX(data->ini) > 0 )
      portRX = wDigInt.getudpportRX(data->ini);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  udp address      [%s]", wDigInt.gethost(data->ini) );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  udp tx port      [%d]", portTX );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  udp rx port      [%d]", portRX );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

    data->readUDP = SocketOp.inst( wDigInt.gethost(data->ini), portRX, False, True, False );
    SocketOp.bind(data->readUDP);
    data->writeUDP = SocketOp.inst( wDigInt.gethost(data->ini), portTX, False, True, False );
    data->conOK = True;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  device           [%s]", wDigInt.getdevice(data->ini) );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  bps              [%d]", wDigInt.isasciiprotocol( data->ini )?115200:500000 );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  SLCAN            [%s]", wDigInt.isasciiprotocol( data->ini )?"yes":"no" );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

    data->serial = SerialOp.inst( wDigInt.getdevice(data->ini) );
    SerialOp.setFlow( data->serial, cts );
    SerialOp.setLine( data->serial, wDigInt.isasciiprotocol( data->ini )?115200:500000, 8, 1, none, wDigInt.isrtsdisabled(data->ini) );
    SerialOp.setTimeout( data->serial, wDigInt.gettimeout( data->ini ), wDigInt.gettimeout( data->ini ) );
    data->conOK = SerialOp.open( data->serial );
  }

  data->fbmod  = wDigInt.getfbmod( ini );
  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );
  data->swtime = wDigInt.getswtime( ini );
  data->run    = True;
  data->readerrun = True;
  data->rprtchnrun = True;
  data->ms2UID = 0;
  data->gbUID  = 0;
  data->mcs2guiUID = 0;
  data->mcs2gfpUID = 0;
  data->sid = 0;

  __clearRegMfxVar(data);
  
  char* readerthreadname = allocMem(16);
  char* writerthreadname = allocMem(16);
  char* fbreaderthreadname = allocMem(16);
  char* mgboxsvcthreadname = allocMem(16);

  if( instCnt==0 ) {
    StrOp.fmtb( readerthreadname, "gbxreader");
    StrOp.fmtb( writerthreadname, "gbxwriter");
    StrOp.fmtb( fbreaderthreadname, "fbreader");
    StrOp.fmtb( mgboxsvcthreadname, "mgboxsvc");
  }
  else {
    StrOp.fmtb( readerthreadname, "%u_gbxreader", instCnt);
    StrOp.fmtb( writerthreadname, "%u_gbxwriter", instCnt);
    StrOp.fmtb( fbreaderthreadname, "%u_fbreader", instCnt);
    StrOp.fmtb( mgboxsvcthreadname, "%u_mgboxsvc", instCnt);
  }

  data->reader = ThreadOp.inst( readerthreadname, &__reader, __MCS2 );
  ThreadOp.start( data->reader );

  data->writer = ThreadOp.inst( writerthreadname, &__writer, __MCS2 );
  ThreadOp.start( data->writer );

  data->feedbackReader = ThreadOp.inst( fbreaderthreadname, &__feedbackMCS2Reader, __MCS2 );
  ThreadOp.start( data->feedbackReader );

  data->mgboxsvc = ThreadOp.inst( mgboxsvcthreadname, &__xS2Svc, __MCS2 );
  ThreadOp.start( data->mgboxsvc );

  instCnt++;
  return __MCS2;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/mcs2.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
