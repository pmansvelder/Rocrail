/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2013 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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

/*
 * Rocrail  BiDiB
 * -------+---------
 * bus    | UniqueID
 * addr   | offset
 */


#include "rocdigs/impl/bidib_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/BiDiB.h"
#include "rocrail/wrapper/public/BiDiBnode.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Command.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/FbInfo.h"
#include "rocrail/wrapper/public/FbMods.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/Clock.h"
#include "rocrail/wrapper/public/Accessory.h"

#include "rocdigs/impl/bidib/bidib_messages.h"
#include "rocdigs/impl/bidib/serial.h"
#include "rocdigs/impl/bidib/udp.h"
#include "rocdigs/impl/bidib/bidibutils.h"

#include <time.h>

static int instCnt = 0;


static void __SoD( iOBiDiB inst, iOBiDiBNode bidibnode );


/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOBiDiBData data = Data(inst);
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

/** ----- OBiDiB ----- */

static void __fx2fn(iOSlot slot, int fx) {
  int i = 0;
  for( i = 0; i < 28; i++ ) {
    slot->f[i] = (fx & (1 << i)) ? True:False;
  }
}

Boolean __getFState(iONode fcmd, int fn) {
  switch( fn ) {
    case 0 : return (wFunCmd.isf0 (fcmd) | wLoc.isfn(fcmd));
    case 1 : return wFunCmd.isf1 (fcmd);
    case 2 : return wFunCmd.isf2 (fcmd);
    case 3 : return wFunCmd.isf3 (fcmd);
    case 4 : return wFunCmd.isf4 (fcmd);
    case 5 : return wFunCmd.isf5 (fcmd);
    case 6 : return wFunCmd.isf6 (fcmd);
    case 7 : return wFunCmd.isf7 (fcmd);
    case 8 : return wFunCmd.isf8 (fcmd);
    case 9 : return wFunCmd.isf9 (fcmd);
    case 10: return wFunCmd.isf10(fcmd);
    case 11: return wFunCmd.isf11(fcmd);
    case 12: return wFunCmd.isf12(fcmd);
    case 13: return wFunCmd.isf13(fcmd);
    case 14: return wFunCmd.isf14(fcmd);
    case 15: return wFunCmd.isf15(fcmd);
    case 16: return wFunCmd.isf16(fcmd);
    case 17: return wFunCmd.isf17(fcmd);
    case 18: return wFunCmd.isf18(fcmd);
    case 19: return wFunCmd.isf19(fcmd);
    case 20: return wFunCmd.isf20(fcmd);
    case 21: return wFunCmd.isf21(fcmd);
    case 22: return wFunCmd.isf22(fcmd);
    case 23: return wFunCmd.isf23(fcmd);
    case 24: return wFunCmd.isf24(fcmd);
    case 25: return wFunCmd.isf25(fcmd);
    case 26: return wFunCmd.isf26(fcmd);
    case 27: return wFunCmd.isf27(fcmd);
    case 28: return wFunCmd.isf28(fcmd);
  }
  return False;
}

static iOSlot __getSlot(iOBiDiB inst, iONode node) {
  iOBiDiBData data = Data(inst);
  int    addr  = wLoc.getaddr(node);
  iOSlot slot  = NULL;
  byte* packet = NULL;

  slot = (iOSlot)MapOp.get( data->lcmap, wLoc.getid(node) );
  if( slot != NULL ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "slot exist for %s", wLoc.getid(node) );
    return slot;
  }

  slot = allocMem( sizeof( struct slot) );
  slot->addr    = addr;
  slot->id      = StrOp.dup(wLoc.getid(node));
  slot->f[0]    = wLoc.isfn(node);
  slot->dir     = wLoc.isdir(node);

  __fx2fn(slot, wLoc.getfx(node));


  if( MutexOp.wait( data->lcmux ) ) {
    MapOp.put( data->lcmap, wLoc.getid(node), (obj)slot);
    MutexOp.post(data->lcmux);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot created for %s", wLoc.getid(node) );
  return slot;
}

static iOSlot __getSlotByAddr(iOBiDiB inst, int lcaddr) {
  iOBiDiBData data = Data(inst);
  iOSlot slot = NULL;
  if( MutexOp.wait( data->lcmux ) ) {
    slot = (iOSlot)MapOp.first( data->lcmap);
    while( slot != NULL ) {
      if( slot->addr == lcaddr ) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "slot found for %s by address %d", slot->id, lcaddr );
        break;
      }
      slot = (iOSlot)MapOp.next( data->lcmap);
    };
    MutexOp.post(data->lcmux);
  }
  return slot;
}


static void __uid2Array(int uid, byte* b) {
  b[3] = (uid & 0x000000FF);
  b[2] = (uid & 0x0000FF00) >> 8;
  b[1] = (uid & 0x00FF0000) >> 16;
  b[0] = (uid & 0xFF000000) >> 24;
}

static iONode __getIniNode(iOBiDiB bidib, int uid) {
  iOBiDiBData data = Data(bidib);
  iONode child = wBiDiB.getbidibnode(data->bidibini);
  while( child != NULL ) {
    if( wBiDiBnode.getuid(child) == uid )
      return child;
    child = wBiDiB.nextbidibnode(data->bidibini, child);
  }
  return NULL;
}


static void __inform( iOBiDiB inst ) {
  iOBiDiBData data = Data(inst);
  iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
  wState.setiid( node, wDigInt.getiid( data->ini ) );
  wState.setpower( node, data->power );
  data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
}

static void __resetSeq( iOBiDiB inst ) {
  iOBiDiBData data = Data(inst);

  iOBiDiBNode node = (iOBiDiBNode)MapOp.first(data->nodemap);
  while(node != NULL) {
    node->seq = 0;
    node = (iOBiDiBNode)MapOp.next(data->nodemap);
  }
}


static void __SoD4Node( iOBiDiB inst, iOBiDiBNode node, Boolean force ) {
  iOBiDiBData data = Data(inst);
  byte msgdata[127];

  if( !force && node->sod )
    return;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Start of Day: uid=0x%08X %d sensors", node->uid, node->sensorcnt );
  ThreadOp.sleep(10);

  msgdata[0] = 0; // address range
  msgdata[1] = node->sensorcnt;
  data->subWrite((obj)inst, node->path, MSG_BM_GET_RANGE, msgdata, 2, node->seq++);
  node->sod = True;
  ThreadOp.sleep(10);

  if( !force ) {
    data->subWrite((obj)inst, node->path, MSG_SYS_ENABLE, NULL, 0, node->seq++);
    ThreadOp.sleep(10);

    if( data->secAck && data->secAckInt > 0 ) {
      // MSG_FEATURE_SET
      msgdata[0] = FEATURE_BM_SECACK_AVAILABLE;
      msgdata[1] = 1;
      data->subWrite((obj)inst, node->path, MSG_FEATURE_SET, msgdata, 2, node->seq++);

      node->pendingfeature = FEATURE_BM_SECACK_ON;
      msgdata[0] = FEATURE_BM_SECACK_ON;
      msgdata[1] = data->secAckInt;
      data->subWrite((obj)inst, node->path, MSG_FEATURE_SET, msgdata, 2, node->seq++);
    }
    else {
      node->pendingfeature = FEATURE_BM_SECACK_ON;
      msgdata[0] = FEATURE_BM_SECACK_ON;
      msgdata[1] = 0;
      data->subWrite((obj)inst, node->path, MSG_FEATURE_SET, msgdata, 2, node->seq++);
    }
  }
}


static void __SoD( iOBiDiB inst, iOBiDiBNode bidibnode ) {
  iOBiDiBData data = Data(inst);
  byte msgdata[127];

  if( bidibnode != NULL ) {
    __SoD4Node(inst, bidibnode, False);
    return;
  }

  iOBiDiBNode node = (iOBiDiBNode)MapOp.first(data->nodemap);
  while(node != NULL) {
    if( node->classid & 0x40 ) {

      if( node->sensorcnt == 0 ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "get FEATURE_BM_SIZE for node uid=0x%08X", node->uid );
        node->pendingfeature = FEATURE_BM_SIZE;
        msgdata[0] = FEATURE_BM_SIZE;
        data->subWrite((obj)inst, node->path, MSG_FEATURE_GET, msgdata, 1, node->seq++);
        ThreadOp.sleep(10);
        node = (iOBiDiBNode)MapOp.next(data->nodemap);
        continue;
      }

      __SoD4Node(inst, node, True);
      ThreadOp.sleep(10);
    }
    node = (iOBiDiBNode)MapOp.next(data->nodemap);
  }
}


static Boolean __accessoryCommand(iOBiDiB inst, iONode node) {
  iOBiDiBData data = Data(inst);
  byte msgdata[32];
  char uidKey[32];
  int cmd = wProgram.getcmd(node);
  iOBiDiBNode bidibnode = NULL;
  StrOp.fmtb( uidKey, "0x%08X", wProgram.getmodid(node) );
  bidibnode = (iOBiDiBNode)MapOp.get( data->nodemap, uidKey );
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "accessory command %d for node %s", cmd, uidKey );

  if( bidibnode != NULL ) {
    if( cmd == wProgram.acc_setparam ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set accessory param %d", wProgram.getcv(node) );
      if( wProgram.getcv(node) == BIDIB_ACCESSORY_SWITCH_TIME ) {
        msgdata[0] = wProgram.getaddr(node);
        msgdata[1] = BIDIB_ACCESSORY_SWITCH_TIME;
        msgdata[2] = wProgram.getval1(node);
        data->subWrite((obj)inst, bidibnode->path, MSG_ACCESSORY_PARA_SET, msgdata, 3, bidibnode->seq++);
      }
      else if( wProgram.getcv(node) == BIDIB_ACCESSORY_PARA_MACROMAP ) {
        msgdata[ 0] = wProgram.getaddr(node);
        msgdata[ 1] = BIDIB_ACCESSORY_PARA_MACROMAP;
        msgdata[ 2] = wProgram.getval1(node);
        msgdata[ 3] = wProgram.getval2(node);
        msgdata[ 4] = wProgram.getval3(node);
        msgdata[ 5] = wProgram.getval4(node);
        msgdata[ 6] = wProgram.getval5(node);
        msgdata[ 7] = wProgram.getval6(node);
        msgdata[ 8] = wProgram.getval7(node);
        msgdata[ 9] = wProgram.getval8(node);
        msgdata[10] = wProgram.getval9(node);
        msgdata[11] = wProgram.getval10(node);
        msgdata[12] = wProgram.getval11(node);
        msgdata[13] = wProgram.getval12(node);
        msgdata[14] = wProgram.getval13(node);
        msgdata[15] = wProgram.getval14(node);
        msgdata[16] = wProgram.getval15(node);
        msgdata[17] = wProgram.getval16(node);
        data->subWrite((obj)inst, bidibnode->path, MSG_ACCESSORY_PARA_SET, msgdata, 18, bidibnode->seq++);
      }
    }
    else if( cmd == wProgram.acc_getparam ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "get accessory param %d", wProgram.getcv(node) );
      msgdata[0] = wProgram.getaddr(node);
      msgdata[1] = wProgram.getcv(node);
      data->subWrite((obj)inst, bidibnode->path, MSG_ACCESSORY_PARA_GET, msgdata, 2, bidibnode->seq++);
    }
    return True;
  }

  return False;
}


static Boolean __macroCommand(iOBiDiB inst, iONode node) {
  iOBiDiBData data = Data(inst);
  byte msgdata[32];
  char uidKey[32];
  int cmd = wProgram.getcmd(node);
  iOBiDiBNode bidibnode = NULL;
  StrOp.fmtb( uidKey, "0x%08X", wProgram.getmodid(node) );
  bidibnode = (iOBiDiBNode)MapOp.get( data->nodemap, uidKey );
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "macro command %d for node %s", cmd, uidKey );

  if( bidibnode != NULL ) {
      if( cmd == wProgram.macro_restore ) {
        /* MSG_LC_MACRO_HANDLE (index, BIDIB_MACRO_RESTORE) */
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "restore macro %d", wProgram.getval1(node) );
        msgdata[0] = wProgram.getval1(node);
        msgdata[1] = BIDIB_MACRO_RESTORE;
        data->subWrite((obj)inst, bidibnode->path, MSG_LC_MACRO_HANDLE, msgdata, 2, bidibnode->seq++);
      }

      else if( cmd == wProgram.macro_save ) {
        /* MSG_LC_MACRO_HANDLE (index, BIDIB_MACRO_SAVE) */
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "save macro %d", wProgram.getval1(node) );
        msgdata[0] = wProgram.getval1(node);
        msgdata[1] = BIDIB_MACRO_SAVE;
        data->subWrite((obj)inst, bidibnode->path, MSG_LC_MACRO_HANDLE, msgdata, 2, bidibnode->seq++);
      }

      else if( cmd == wProgram.macro_delete ) {
        /* MSG_LC_MACRO_HANDLE (index, BIDIB_MACRO_DELETE) */
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "delete macro %d", wProgram.getval1(node) );
        msgdata[0] = wProgram.getval1(node);
        msgdata[1] = BIDIB_MACRO_DELETE;
        data->subWrite((obj)inst, bidibnode->path, MSG_LC_MACRO_HANDLE, msgdata, 2, bidibnode->seq++);
      }

      else if( cmd == wProgram.macro_set ) {
        /* MSG_LC_MACRO_SET (index, line, delay, ptype, portnum, portstat ) */
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set macro %d: line=%d, delay=%d, type=%d, port=%d, stat=%d",
            wProgram.getval1(node), wProgram.getval2(node), wProgram.getval3(node),
            wProgram.getval4(node), wProgram.getval5(node), wProgram.getval6(node) );
        msgdata[0] = wProgram.getval1(node);
        msgdata[1] = wProgram.getval2(node);
        msgdata[2] = wProgram.getval3(node);
        msgdata[3] = wProgram.getval4(node);
        msgdata[4] = wProgram.getval5(node);
        msgdata[5] = wProgram.getval6(node);
        data->subWrite((obj)inst, bidibnode->path, MSG_LC_MACRO_SET, msgdata, 6, bidibnode->seq++);
      }

      else if( cmd == wProgram.macro_get ) {
        /* MSG_LC_MACRO_GET (index, line ) */
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "get macro %d: line=%d",
            wProgram.getval1(node), wProgram.getval2(node) );
        msgdata[0] = wProgram.getval1(node);
        msgdata[1] = wProgram.getval2(node);
        data->subWrite((obj)inst, bidibnode->path, MSG_LC_MACRO_GET, msgdata, 2, bidibnode->seq++);
      }

      else if( cmd == wProgram.macro_setparams ) {
        /* MSG_LC_MACRO_PARA_SET (index, parameter, valLSB, valM1, valM2, valMSB ) */
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set macro %d parameter %d: valLSB=%d, valM1=%d, valM2=%d, valMSB=%d",
            wProgram.getval1(node), wProgram.getval2(node), wProgram.getval3(node),
            wProgram.getval4(node), wProgram.getval5(node), wProgram.getval6(node) );
        msgdata[0] = wProgram.getval1(node);
        msgdata[1] = wProgram.getval2(node);
        msgdata[2] = wProgram.getval3(node);
        msgdata[3] = wProgram.getval4(node);
        msgdata[4] = wProgram.getval5(node);
        msgdata[5] = wProgram.getval6(node);
        data->subWrite((obj)inst, bidibnode->path, MSG_LC_MACRO_PARA_SET, msgdata, 6, bidibnode->seq++);
      }

      else if( cmd == wProgram.macro_getparams ) {
        /* MSG_LC_MACRO_PARA_GET (index, parameter ) */
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "get macro %d paramter %d",
            wProgram.getval1(node), wProgram.getval2(node) );
        msgdata[0] = wProgram.getval1(node);
        msgdata[1] = wProgram.getval2(node);
        data->subWrite((obj)inst, bidibnode->path, MSG_LC_MACRO_PARA_GET, msgdata, 2, bidibnode->seq++);
      }

      return True;
    }

    return False;
  }



static iONode __translate( iOBiDiB inst, iONode node ) {
  iOBiDiBData data = Data(inst);
  iOBiDiBNode bidibnode = NULL;
  iONode rsp = NULL;
  char uidKey[32];
  byte path[4] = {0,0,0,0};
  byte msgdata[127];
  iOBiDiBNode broadcastnode = (iOBiDiBNode)MapOp.get( data->localmap, "0.0.0.0");

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "cmd=%s", NodeOp.getName( node ) );

  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );
    StrOp.fmtb( uidKey, "0x%08X", wSysCmd.getbus(node) );
    bidibnode = (iOBiDiBNode)MapOp.get( data->nodemap, uidKey );

    if( bidibnode != NULL ) {
      /* Direct Booster command */
      if( StrOp.equals( cmd, wSysCmd.go ) ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Booster Power ON" );
        msgdata[0] = 1;
        data->subWrite((obj)inst, bidibnode->path, MSG_BOOST_ON, msgdata, 1, bidibnode->seq++);
        return rsp;
      }
      if( StrOp.equals( cmd, wSysCmd.stop ) ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Booster Power OFF" );
        msgdata[0] = 1;
        data->subWrite((obj)inst, bidibnode->path, MSG_BOOST_OFF, msgdata, 1, bidibnode->seq++);
        return rsp;
      }
      if( StrOp.equals( cmd, wSysCmd.resetstat ) ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Booster reset statistics" );
        bidibnode->loadmax = 0;
        bidibnode->tempmax = 0;
        bidibnode->voltmin = 0;
        return rsp;
      }
    }

    if( bidibnode == NULL ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "using default booster..." );
      bidibnode = data->defaultbooster;
    }

    if( bidibnode == NULL ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "no command station available for command=%s; broadcast it to 0.0.0.0", cmd );
      bidibnode = (iOBiDiBNode)MapOp.get( data->localmap, "0.0.0.0" );
    }

    if( data->defaultmain != NULL && StrOp.equals( cmd, wSysCmd.stop ) ) {
      bidibnode = data->defaultmain;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      msgdata[0] = BIDIB_CS_STATE_OFF;
      data->subWrite((obj)inst, bidibnode->path, MSG_CS_SET_STATE, msgdata, 1, bidibnode->seq++);
      data->power = False;
      __inform(inst);
    }
    else if( data->defaultmain != NULL && StrOp.equals( cmd, wSysCmd.go ) ) {
      bidibnode = data->defaultmain;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      msgdata[0] = BIDIB_CS_STATE_GO;
      data->subWrite((obj)inst, bidibnode->path, MSG_CS_SET_STATE, msgdata, 1, bidibnode->seq++);
      data->power = True;
      __inform(inst);
    }
    else if( data->defaultmain != NULL && StrOp.equals( cmd, wSysCmd.ebreak ) ) {
      bidibnode = data->defaultmain;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Emergency break" );
      msgdata[0] = BIDIB_CS_STATE_STOP;
      data->subWrite((obj)inst, bidibnode->path, MSG_CS_SET_STATE, msgdata, 1, bidibnode->seq++);
      data->power = False;
      __inform(inst);
    }
    else if( StrOp.equals( cmd, wSysCmd.sod ) ) {
      __SoD(inst, NULL);
    }
  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int delay = wSwitch.getdelay(node) > 0 ? wSwitch.getdelay(node):data->swtime;
    int addr = wSwitch.getaddr1( node );

    StrOp.fmtb( uidKey, "0x%08X", wSwitch.getbus(node) );
    bidibnode = (iOBiDiBNode)MapOp.get( data->nodemap, uidKey );
    if( bidibnode == NULL ) {
      if( wSwitch.isaccessory(node) && StrOp.equals( wSwitch.getprot( node ), wSwitch.prot_N ) )
        bidibnode = data->defaultmain;
      else
        bidibnode = data->defaultswitch;
    }
    if( bidibnode != NULL ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "switch %d:%d %s",
          wSwitch.getbus( node ), wSwitch.getaddr1( node ), wSwitch.getcmd(node) );

      if( wSwitch.isaccessory(node) && StrOp.equals( wSwitch.getprot( node ), wSwitch.prot_N ) ) {
        if( wSwitch.issinglegate(node) ) {
          int gate = wSwitch.getgate1(node);
          msgdata[0] = (addr-1) % 256;
          msgdata[1] = (addr-1) / 256;
          msgdata[2] = (StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node)) ? 0x20:0x00) + gate;
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "DCC accessory %d:%d.%d single gate %s",
              wSwitch.getbus( node ), addr, gate, StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node)) ? "ON":"OFF" );
          data->subWrite((obj)inst, bidibnode->path, MSG_CS_ACCESSORY, msgdata, 3, bidibnode->seq++);
        }
        else {
          msgdata[0] = (addr-1) % 256;
          msgdata[1] = (addr-1) / 256;
          msgdata[2] = 0x60 + (StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node)) ? 0x00:0x01);
          /* Schaltzeit
              Bit Bedeutung
              7 Grundeinheit:
              0: 100ms (d.h. Wertebereich 0...12,7s)
              1: 1s (d.h. Wertebereich 0...127s)
              6...0 Schaltzeit, Wertebereich 0..127
          */
          delay /= 100;
          if( delay > 127 )
            delay = 0x80 + (delay/10);
          msgdata[3] = delay;
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "DCC accessory %d:%d set to %s",
              wSwitch.getbus( node ), addr, wSwitch.getcmd(node) );
          data->subWrite((obj)inst, bidibnode->path, MSG_CS_ACCESSORY, msgdata, 4, bidibnode->seq++);
        }
      }
      else if( wSwitch.isaccessory(node) ) {
        if( wSwitch.issinglegate(node) ) {
          msgdata[0] = addr-1; // Null offset.
          msgdata[1] = StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node)) ? 1:0;
        }
        else {
          msgdata[0] = StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node)) ? addr:addr-1;
          msgdata[1] = 1;
        }
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "accessory %d:%d set to %d (%s)",
            wSwitch.getbus( node ), msgdata[0], msgdata[1], wSwitch.getcmd(node) );
        data->subWrite((obj)inst, bidibnode->path, MSG_ACCESSORY_SET, msgdata, 2, bidibnode->seq++);
      }
      else {
        if( wSwitch.getporttype(node) == wProgram.porttype_macro ) {
          if( wSwitch.issinglegate(node) ) {
            msgdata[0] = addr-1; // Null offset.
            msgdata[1] = StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node)) ? BIDIB_MACRO_START:BIDIB_MACRO_OFF;
          }
          else {
            msgdata[0] = StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node)) ? addr-1:addr;
            msgdata[1] = BIDIB_MACRO_START;
          }
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "macro %d:%d set to %d",
              wSwitch.getbus( node ), msgdata[0], msgdata[1] );
          data->subWrite((obj)inst, bidibnode->path, MSG_LC_MACRO_HANDLE, msgdata, 2, bidibnode->seq++);
        }
        else {
          msgdata[0] = wSwitch.getporttype(node);
          if( wSwitch.issinglegate(node) ) {
            msgdata[1] = addr-1; // Null offset.
            if( msgdata[0] == BIDIB_OUTTYPE_SERVO )
              msgdata[2] = StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node)) ? 255:0;
            else
              msgdata[2] = StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node)) ? BIDIB_PORT_TURN_ON:BIDIB_PORT_TURN_OFF;
          }
          else {
            msgdata[1] = StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node)) ? addr-1:addr;
            msgdata[2] = BIDIB_PORT_TURN_ON; // ToDo: Switch off other coil.
          }
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "lc %d:%d type %d set to %d",
              wSwitch.getbus( node ), msgdata[1], msgdata[0], msgdata[2] );
          data->subWrite((obj)inst, bidibnode->path, MSG_LC_OUTPUT, msgdata, 3, bidibnode->seq++);
        }
      }

    }
    else {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "accessory node %s is unknown (%s)", uidKey, wSwitch.getid(node) );
    }
  }

  /* Signal command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSignal.name() ) ) {
    byte cmd[5];
    int addr = wSignal.getaddr(node);
    int aspect = wSignal.getaspect(node);

    StrOp.fmtb( uidKey, "0x%08X", wSignal.getbus(node) );
    bidibnode = (iOBiDiBNode)MapOp.get( data->nodemap, uidKey );
    if( bidibnode == NULL ) {
        bidibnode = data->defaultmain;
    }
    if( bidibnode != NULL ) {
      if( wSignal.isaccessory(node) && StrOp.equals( wSignal.getprot( node ), wSwitch.prot_N ) ) {
        msgdata[0] = (addr-1) % 256;
        msgdata[1] = (addr-1) / 256;
        msgdata[2] = 0x80 + 0x20 + aspect;
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "extended DCC accessory %d:%d aspect=%d",
            wSwitch.getbus( node ), addr, aspect );
        data->subWrite((obj)inst, bidibnode->path, MSG_CS_ACCESSORY, msgdata, 3, bidibnode->seq++);
      }
      else {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "signal %d:%d aspect %d",
            wSignal.getbus( node ), wSignal.getaddr( node ), aspect );

        msgdata[0] = addr-1; // Null offset.
        msgdata[1] = aspect;
        data->subWrite((obj)inst, bidibnode->path, MSG_ACCESSORY_SET, msgdata, 2, bidibnode->seq++);
      }
    }
  }


  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    byte cmd[5];
    Boolean on = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;
    int addr = wOutput.getaddr( node );

    StrOp.fmtb( uidKey, "0x%08X", wOutput.getbus(node) );
    bidibnode = (iOBiDiBNode)MapOp.get( data->nodemap, uidKey );
    if( bidibnode == NULL ) {
      if( wOutput.isaccessory(node) && StrOp.equals( wOutput.getprot( node ), wSwitch.prot_N ) )
        bidibnode = data->defaultmain;
      else
        bidibnode = data->defaultswitch;
    }
    if( bidibnode != NULL ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output %d:%d %s",
          wOutput.getbus( node ), wOutput.getaddr( node ), on?"ON":"OFF" );

      if( wOutput.isaccessory(node) && StrOp.equals( wOutput.getprot( node ), wSwitch.prot_N ) ) {
        if(StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node)))
          addr++;
        msgdata[0] = (addr-1) % 256;
        msgdata[1] = (addr-1) / 256;
        msgdata[2] = on ? 0x20:0x00;
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "DCC accessory %d:%d %s",
            wSwitch.getbus( node ), addr, on ? "ON":"OFF" );
        data->subWrite((obj)inst, bidibnode->path, MSG_CS_ACCESSORY, msgdata, 3, bidibnode->seq++);
      }
      else if( wOutput.isaccessory(node) ) {
        msgdata[0] = addr-1; // Null offset.
        msgdata[1] = on ? 1:0;
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "accessory %d:%d set to %d",
            wSwitch.getbus( node ), msgdata[0], msgdata[1] );
        data->subWrite((obj)inst, bidibnode->path, MSG_ACCESSORY_SET, msgdata, 2, bidibnode->seq++);
      }
      else {
        if( wOutput.getporttype(node) == wProgram.porttype_macro ) {
          msgdata[0] = addr-1; // Null offset.
          msgdata[1] = on ? BIDIB_MACRO_START:BIDIB_MACRO_OFF;
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "macro %d:%d set to %d",
              wSwitch.getbus( node ), msgdata[0], msgdata[1] );
          data->subWrite((obj)inst, bidibnode->path, MSG_LC_MACRO_HANDLE, msgdata, 2, bidibnode->seq++);
        }
        else if( wOutput.getporttype(node) == wProgram.porttype_light ) {
          Boolean blink = wOutput.isblink( node );
          int blinkcmd = wOutput.getparam( node );
          if( blinkcmd == 0 || blinkcmd > 9 )
            blinkcmd = BIDIB_PORT_BLINK_A;

          msgdata[0] = wOutput.getporttype(node);
          msgdata[1] = addr-1;
          msgdata[2] = on ? (blink?blinkcmd:BIDIB_PORT_TURN_ON):BIDIB_PORT_TURN_OFF;

          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "lc %d:%d type %d set to %d%s",
              wSwitch.getbus( node ), msgdata[1], msgdata[0], msgdata[2], blink?" blink":"" );
          data->subWrite((obj)inst, bidibnode->path, MSG_LC_OUTPUT, msgdata, 3, bidibnode->seq++);
        }
        else {
          msgdata[0] = wOutput.getporttype(node);
          msgdata[1] = addr-1;
          if( msgdata[0] == BIDIB_OUTTYPE_SERVO )
            msgdata[2] = on ? 255:0;
          else
            msgdata[2] = on ? BIDIB_PORT_TURN_ON:BIDIB_PORT_TURN_OFF;

          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "lc %d:%d type %d set to %d",
              wSwitch.getbus( node ), msgdata[1], msgdata[0], msgdata[2] );
          data->subWrite((obj)inst, bidibnode->path, MSG_LC_OUTPUT, msgdata, 3, bidibnode->seq++);
        }
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "accessory node %s is unknown (%s)", uidKey, wOutput.getid(node) );
    }
  }

  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int   addr = wLoc.getaddr( node );
    int  steps = wLoc.getspcnt( node );
    int  speed = 0;
    Boolean fn  = wLoc.isfn( node );
    Boolean dir = wLoc.isdir( node ); /* True == forwards */
    iOSlot slot = __getSlot(inst, node);

    /*
     * Das MSB kodiert die Fahrtrichtung: 1 = vorwärts, 0 = rückwärts
     * Die LSBs (Bit 6..0) kodieren die Geschwindigkeit, wobei 0 = Halt und 1 = Nothalt bedeutet.
     * Werte von 2 ... 127 bezeichnen die Fahrstufe.
     */
    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * 126) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * 126) / wLoc.getV_max( node );
    }

    if( speed >= 1 ) {
      /* skip emergancy break */
      speed++;
    }

    slot->f[0] = fn;
    slot->V    = speed;
    slot->dir  = dir;

    StrOp.fmtb( uidKey, "0x%08X", wLoc.getbus(node) );
    bidibnode = (iOBiDiBNode)MapOp.get( data->nodemap, uidKey );
    if( bidibnode == NULL )
      bidibnode = data->defaultmain;

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco %d V=%d dir=%s lights=%s steps=%d",
        addr, speed, dir?"fwd":"rev", fn?"on":"off", steps);
    if( bidibnode != NULL ) {
      msgdata[0] = addr % 256;
      msgdata[1] = addr / 256;
      msgdata[2] = (steps==128?0x03:0x02); // 128 speed steps
      msgdata[3] = 0x03; // speed and function group 1
      msgdata[4] = (dir ? 0x80:0x00) + speed;
      msgdata[5] = (fn?0x10:0x00) + (slot->f[1]?0x01:0x00) + (slot->f[2]?0x02:0x00) + (slot->f[3]?0x04:0x00) + (slot->f[4]?0x08:0x00);
      msgdata[6] = 0;
      msgdata[7] = 0;
      msgdata[8] = 0;
      data->subWrite((obj)inst, bidibnode->path, MSG_CS_DRIVE, msgdata, 9, bidibnode->seq++);
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "no booster available");
    }

  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int     addr = wFunCmd.getaddr( node );
    Boolean fn   = wFunCmd.isf0( node );
    iOSlot slot = __getSlot(inst, node);

    int fnchanged = wFunCmd.getfnchanged(node);
    Boolean fnstate = __getFState(node, fnchanged);

    slot->f[fnchanged] = fnstate;

    bidibnode = (iOBiDiBNode)MapOp.get( data->nodemap, uidKey );
    if( bidibnode == NULL )
      bidibnode = data->defaultmain;

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco %d f[%d]=%s", addr, fnchanged, fnstate?"on":"off");
    if( bidibnode != NULL ) {
      msgdata[0] = addr % 256;
      msgdata[1] = addr / 256;
      msgdata[2] = 0;
      msgdata[3] = 0x3E; // functions only
      msgdata[4] = 0;
      msgdata[5] = (fn?0x10:0x00) + (wFunCmd.isf1(node)?0x01:0x00) + (wFunCmd.isf2(node)?0x02:0x00) + (wFunCmd.isf3(node)?0x04:0x00) + (wFunCmd.isf4(node)?0x08:0x00);
      msgdata[6] = (wFunCmd.isf5(node)?0x01:0x00) + (wFunCmd.isf6(node)?0x02:0x00) + (wFunCmd.isf7(node)?0x04:0x00) + (wFunCmd.isf8(node)?0x08:0x00);
      msgdata[6]+= (wFunCmd.isf9(node)?0x10:0x00) + (wFunCmd.isf10(node)?0x20:0x00) + (wFunCmd.isf11(node)?0x40:0x00) + (wFunCmd.isf12(node)?0x80:0x00);
      msgdata[7] = (wFunCmd.isf13(node)?0x01:0x00) + (wFunCmd.isf14(node)?0x02:0x00) + (wFunCmd.isf15(node)?0x04:0x00) + (wFunCmd.isf16(node)?0x08:0x00);
      msgdata[7]+= (wFunCmd.isf17(node)?0x10:0x00) + (wFunCmd.isf18(node)?0x20:0x00) + (wFunCmd.isf19(node)?0x40:0x00) + (wFunCmd.isf20(node)?0x80:0x00);
      msgdata[8] = (wFunCmd.isf21(node)?0x01:0x00) + (wFunCmd.isf22(node)?0x02:0x00) + (wFunCmd.isf23(node)?0x04:0x00) + (wFunCmd.isf24(node)?0x08:0x00);
      msgdata[8]+= (wFunCmd.isf25(node)?0x10:0x00) + (wFunCmd.isf26(node)?0x20:0x00) + (wFunCmd.isf27(node)?0x40:0x00) + (wFunCmd.isf28(node)?0x80:0x00);
      data->subWrite((obj)inst, bidibnode->path, MSG_CS_DRIVE, msgdata, 9, bidibnode->seq++);
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "no booster available");
    }
  }

  /* Sensor command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFeedback.name() ) ) {
    int addr = wFeedback.getaddr( node );
    Boolean state = wFeedback.isstate( node );

    if( wFeedback.isactivelow(node) )
      wFeedback.setstate( node, !state);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "simulate fb addr=%d state=%s", addr, state?"true":"false" );
    rsp = (iONode)NodeOp.base.clone( node );
  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,"program type=%d cmd=%d uid=0x%08X",
        wProgram.getlntype(node), wProgram.getcmd( node ), wProgram.getmodid(node) );

    if( wProgram.getlntype(node) == wProgram.lntype_sv && wProgram.getcmd( node ) == wProgram.lncvget &&
        wProgram.getcv(node) == 0 && wProgram.getmodid(node) == 0 && wProgram.getaddr(node) == 0 )
    {
      // Query all modules.
      iOBiDiBNode bidibnode = (iOBiDiBNode)MapOp.first(data->nodemap);
      while(bidibnode != NULL) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,"uid=%08X path=%d.%d.%d.%d vid=%02X class=%02X",
            bidibnode->uid, bidibnode->path[0], bidibnode->path[1], bidibnode->path[2], bidibnode->path[3],
            bidibnode->vendorid, bidibnode->classid);
        bidibnode = (iOBiDiBNode)MapOp.next(data->nodemap);
      }

    }

    else if( wProgram.getlntype(node) == wProgram.lntype_bidib ) {
      byte msgdata[32];
      char uidKey[32];
      iOBiDiBNode bidibnode = NULL;
      StrOp.fmtb( uidKey, "0x%08X", wProgram.getmodid(node) );
      bidibnode = (iOBiDiBNode)MapOp.get( data->nodemap, uidKey );

      if( bidibnode != NULL ) {
        if( wProgram.getcmd( node ) == wProgram.writehex ) {
          if( data->hexstate == 0 ) {
            if( bidibnode->fwup ) {
              TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
                  "load hex file %s into node %s.", wProgram.getfilename(node), uidKey );
              if( data->hexfile != NULL )
                StrOp.free(data->hexfile);
              data->hexfile = StrOp.dup(wProgram.getfilename(node));
              data->hexline = 0;
              data->hexstate = 1; /* pending */
              data->hexnode = bidibnode;
              msgdata[0] = BIDIB_MSG_FW_UPDATE_OP_ENTER;
              msgdata[1] = bidibnode->classid;
              msgdata[2] = 0;
              msgdata[3] = bidibnode->vendorid;
              __uid2Array(bidibnode->uid, msgdata+4 );
              TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,"MSG_FW_UPDATE_OP -> %08X", bidibnode->uid);
              data->subWrite((obj)inst, bidibnode->path, MSG_FW_UPDATE_OP, msgdata, 8, bidibnode->seq++);
            }
            else {
              iONode rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
              wProgram.setcmd( rsp, wProgram.writehex );
              wProgram.setlntype(rsp, wProgram.lntype_bidib);
              wProgram.setrc( rsp, wProgram.rc_notfwup );
              wProgram.setmodid( rsp, bidibnode->uid );
              if( data->iid != NULL )
                wProgram.setiid( rsp, data->iid );
              if( data->listenerFun != NULL && data->listenerObj != NULL )
                data->listenerFun( data->listenerObj, rsp, TRCLEVEL_INFO );
              TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                  "the update feature flag is not set for node %s; try to read the features first", uidKey );
            }
          }
          else {
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                "loading hex file %s into node %d is pending: line=%d", data->hexfile, data->hexnode->uid, data->hexline );
          }

        }
        else if( wProgram.getcmd( node ) == wProgram.get ) {
          msgdata[0] = wProgram.getcv(node);
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "get feature %d", msgdata[0]);
          data->subWrite((obj)inst, bidibnode->path, MSG_FEATURE_SET, msgdata, 1, bidibnode->seq++);
        }
        else if( wProgram.getcmd( node ) == wProgram.set ) {
          msgdata[0] = wProgram.getcv(node);
          msgdata[1] = wProgram.getvalue(node);
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set feature %d to %d", msgdata[0], msgdata[1]);
          data->subWrite((obj)inst, bidibnode->path, MSG_FEATURE_SET, msgdata, 2, bidibnode->seq++);
        }
        else if( wProgram.getcmd( node ) == wProgram.evgetall ) {
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
              "get all features from module %d.%d.%d.%d %s...",
              bidibnode->path[0], bidibnode->path[1], bidibnode->path[2], bidibnode->path[3], uidKey );
          data->subWrite((obj)inst, bidibnode->path, MSG_FEATURE_GETALL, NULL, 0, bidibnode->seq++);
        }
        else if( wProgram.getcmd( node ) == wProgram.nvset ) {
          msgdata[0] = wProgram.getporttype(node);
          msgdata[1] = wProgram.getcv(node);
          msgdata[2] = wProgram.getval1(node);
          msgdata[3] = wProgram.getval2(node);
          msgdata[4] = wProgram.getval3(node);
          msgdata[5] = wProgram.getval4(node);
          data->subWrite((obj)inst, bidibnode->path, MSG_LC_CONFIG_SET, msgdata, 6, bidibnode->seq++);
        }
        else if( wProgram.getcmd( node ) == wProgram.nvget ) {
          msgdata[0] = wProgram.getporttype(node);
          msgdata[1] = wProgram.getcv(node);
          data->subWrite((obj)inst, bidibnode->path, MSG_LC_CONFIG_GET, msgdata, 2, bidibnode->seq++);
        }
        else if( wProgram.getcmd( node ) >= wProgram.macro_restore && wProgram.getcmd( node ) <= wProgram.macro_getparams ) {
          __macroCommand(inst, node);
        }
        else if( wProgram.getcmd( node ) == wProgram.acc_setparam || wProgram.getcmd( node ) == wProgram.acc_getparam ) {
          __accessoryCommand(inst, node);
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
            "programming: unknown uid=%s.", uidKey );
      }
    }
    else {
      if( data->defaultprog == NULL ) {
        data->defaultprog = data->defaultmain;
      }

      if( wProgram.ispom(node)) {
        if( wProgram.getcmd( node ) == wProgram.get ) {
          int addr = wProgram.getaddr(node);
          data->cv = wProgram.getcv( node );

          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
              "%08X get POM Addr=%d CV%d...", data->defaultprog->uid, addr, data->cv );
          if( data->defaultprog != NULL ) {
            msgdata[0] = addr % 256;
            msgdata[1] = addr / 256;
            msgdata[2] = 0;
            msgdata[3] = 0;
            msgdata[4] = 0;
            msgdata[5] = BIDIB_CS_POM_RD_BYTE;
            msgdata[6] = (data->cv-1) % 256;
            msgdata[7] = (data->cv-1) / 256;
            msgdata[8] = 0;
            msgdata[9] = 0;
            data->subWrite((obj)inst, data->defaultprog->path, MSG_CS_POM, msgdata, 10, data->defaultprog->seq++);
          }
        }
        else if( wProgram.getcmd( node ) == wProgram.set ) {
          int addr = wProgram.getaddr(node);
          data->cv = wProgram.getcv( node );
          data->value = wProgram.getvalue( node );

          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
              "%08X get POM Addr=%d CV%d...", data->defaultprog->uid, addr, data->cv );
          if( data->defaultprog != NULL ) {
            msgdata[0] = addr % 256;
            msgdata[1] = addr / 256;
            msgdata[2] = 0;
            msgdata[3] = 0;
            msgdata[4] = 0;
            msgdata[5] = BIDIB_CS_POM_WR_BYTE;
            msgdata[6] = (data->cv-1) % 256;
            msgdata[7] = (data->cv-1) / 256;
            msgdata[8] = 0;
            msgdata[9] = data->value;
            data->subWrite((obj)inst, data->defaultprog->path, MSG_CS_POM, msgdata, 10, data->defaultprog->seq++);
          }
        }
      }
      else {
        if( wProgram.getcmd( node ) == wProgram.get ) {
          data->cv = wProgram.getcv( node );
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "%08X get CV%d...", data->defaultprog->uid, data->cv );
          if( data->defaultprog != NULL ) {
            msgdata[0] = data->cv % 256;
            msgdata[1] = data->cv / 256;
            data->subWrite((obj)inst, data->defaultprog->path, MSG_PRG_CV_READ, msgdata, 2, data->defaultprog->seq++);
          }
        }
        else if( wProgram.getcmd( node ) == wProgram.set ) {
          data->cv = wProgram.getcv( node );
          data->value = wProgram.getvalue( node );
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "%08X set CV%d to %d...", data->defaultprog->uid, data->cv, data->value );
          if( data->defaultprog != NULL ) {
            msgdata[0] = data->cv % 256;
            msgdata[1] = data->cv / 256;
            msgdata[2] = data->value;
            data->subWrite((obj)inst, data->defaultprog->path, MSG_PRG_CV_WRITE, msgdata, 3, data->defaultprog->seq++);
          }
        }
      }
    }
  }


  /* Clock command. */
  else if( StrOp.equals( NodeOp.getName( node ), wClock.name() ) ) {
    /* Fast Clock */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "clock: %s", wClock.getcmd( node ) );

    if(  StrOp.equals( wClock.getcmd( node ), wClock.freeze ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "freeze clock" );
      wClock.setcmd( node, wClock.set );
      wClock.setdivider( node, 0 );
    }
    else if(  StrOp.equals( wClock.getcmd( node ), wClock.go ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "go clock" );
      wClock.setcmd( node, wClock.set );
    }
    else if(  StrOp.equals( wClock.getcmd( node ), wClock.sync ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "sync clock" );
      if( !data->fcsync ) {
        wClock.setcmd( node, wClock.set );
        data->fcsync = True;
      }
    }

    if(  StrOp.equals( wClock.getcmd( node ), wClock.set ) || StrOp.equals( wClock.getcmd( node ), wClock.sync ) ) {
      int div   = wClock.getdivider(node);
      long l_time = wClock.gettime(node);
      struct tm* lTime = localtime( &l_time );

      int mins  = lTime->tm_min;
      int hours = lTime->tm_hour;
      int wday  = lTime->tm_wday;
      int mday  = lTime->tm_mday;
      int mon   = lTime->tm_mon;

      /* correction for day of week */
      if( wday > 0 )
        wday--;
      else
        wday = 6;

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
         "set clock to %d:%02d wday=%d divider=%d", hours, mins, wday, div );
      /*
      TCODE0  00mmmmmm  mmmmmm = Angabe der Minute, Wertebereich 0...59.
      TCODE1  100HHHHH  HHHHH = Angabe der Stunde, Wertebereich 0...23.
      TCODE2  01000www  www = Wochentag, 0=Montag, 1=Dienstag, ... 6=Sonntag.
      TCODE3  110fffff  fffff = Uhrbeschleunigungsfaktor, fffff=0 heißt Uhr angehalten.
      */
      msgdata[0] = mins;
      msgdata[1] = hours + 0x80;
      msgdata[2] = wday  + 0x40;
      msgdata[3] = div   + 0xC0;

      if( broadcastnode != NULL )
        data->subWrite((obj)inst, broadcastnode->path, MSG_SYS_CLOCK, msgdata, 4, broadcastnode->seq++);
      else
        data->subWrite((obj)inst, path, MSG_SYS_CLOCK, msgdata, 4, 0);
    }
  }


  return rsp;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOBiDiBData data = Data(inst);
  iONode rsp = NULL;

  if( cmd != NULL ) {
    rsp = __translate((iOBiDiB)inst, cmd);
    cmd->base.del(cmd);
  }
  return rsp;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  iOBiDiBData data = Data(inst);
  byte msgdata[4] = {0,0,0,0};
  iOBiDiBNode broadcastnode = (iOBiDiBNode)MapOp.get( data->localmap, "0.0.0.0");


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "halt BiDiB..." );

  if( data->defaultmain != NULL && poweroff ) {
    msgdata[0] = BIDIB_CS_STATE_OFF;
    data->subWrite((obj)inst, data->defaultmain->path, MSG_CS_SET_STATE, msgdata, 1, data->defaultmain->seq++);
    data->power = False;
  }
  if( broadcastnode != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "disable the BiDiBus" );
    data->subWrite((obj)inst, broadcastnode->path, MSG_SYS_DISABLE, NULL, 0, broadcastnode->seq++);
  }

  ThreadOp.sleep(250);
  data->run = False;
  ThreadOp.sleep(500);
  data->subDisconnect(inst);
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOBiDiBData data = Data(inst);
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


/*
 * Type:
 * Bit 15,14  Bits 13...0
 * 00  Lokadresse, Fahrtrichtung vorwärts
 * 10  Lokadresse, Fahrtrichtung rückwärts
 * 01  Accessory-Adresse
 * 11  Extended Accessory
 */
static void __handleSensor(iOBiDiB bidib, int bus, int addr, Boolean state, int locoAddr, int type, int load ) {
  iOBiDiBData data = Data(bidib);
  char ident[32];

  /* Type:
    00  Lokadresse, Fahrtrichtung vorwärts
    10  Lokadresse, Fahrtrichtung rückwärts
    01  Accessory-Adresse
    11  Extended Accessory
  */

  char* sType = "loco-addr-fwd";
  if( type == 2 ) sType = "loco-addr-rev";
  if( type == 1 ) sType = "accessory-addr";
  if( type == 3 ) sType = "ext-accessory";

  addr++; /* increase address with one */
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
      "sensor bus=%08X addr=%d state=%s ident=%d type=%s", bus, addr, state?"occ":"free", locoAddr, sType );

  if( type == -1 || type == 0 || type == 2 ) {
    /* occ event */
    /* inform listener: Node3 */
    iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );

    wFeedback.setbus( nodeC, bus );
    wFeedback.setaddr( nodeC, addr );
    wFeedback.setload( nodeC, load );
    wFeedback.setfbtype( nodeC, wFeedback.fbtype_sensor );

    if( data->iid != NULL )
      wFeedback.setiid( nodeC, data->iid );

    wFeedback.setstate( nodeC, state );
    StrOp.fmtb(ident, "%d", locoAddr);
    wFeedback.setidentifier( nodeC, ident);
    if( type == 0 || type == 2 )
      wFeedback.setdirection( nodeC, type == 0 ? True:False );

    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
  }
  else if( type == 1 || type == 3 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "accessory events not jet supported" );
  }
}


static void __handleMultipleSensors(iOBiDiB bidib, int bus, const byte* pdata, int size) {
  iOBiDiBData data = Data(bidib);

  // 06 00 02 A2 00 08 01 8B
  int baseAddr = pdata[0];
  int cnt = pdata[1] / 8;
  int i = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sensor-base=%d cnt=%d", baseAddr, cnt );
  for( i = 0; i < cnt; i++ ) {
    int addr = baseAddr + (i / 2);
    int bit = 0;
    for( bit = 0; bit < 8; bit++ ) {
      __handleSensor(bidib, bus, bit+((i%2)*8), pdata[2+i] & (0x01 << bit), 0, -1, 0);
    }
  }

}


static void __handleCV(iOBiDiB bidib, int addr, int cv, int val) {
  iOBiDiBData data = Data(bidib);

  iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "CV response" );

  wProgram.setvalue( node, val );
  wProgram.setcmd( node, wProgram.datarsp );
  wProgram.setcv( node, cv );
  wProgram.setdecaddr( node, addr );
  if( data->iid != NULL )
    wProgram.setiid( node, data->iid );

  if( data->listenerFun != NULL && data->listenerObj != NULL )
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );

}


static void __handleMacroGet(iOBiDiB bidib, int uid, byte* pdata ) {
  iOBiDiBData data = Data(bidib);
  iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( node, wProgram.macro_get );
  wProgram.setlntype(node, wProgram.lntype_bidib);
  wProgram.setmodid( node, uid );
  wProgram.setval1( node, pdata[0] );
  wProgram.setval2( node, pdata[1] );
  wProgram.setval3( node, pdata[2] );
  wProgram.setval4( node, pdata[3] );
  wProgram.setval5( node, pdata[4] );
  wProgram.setval6( node, pdata[5] );
  if( data->iid != NULL )
    wProgram.setiid( node, data->iid );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "MSG_LC_MACRO macro %d: line=%d, delay=%d, type=%d, port=%d, stat=%d",
      pdata[0], pdata[1], pdata[2], pdata[3], pdata[4], pdata[5] );

  if( data->listenerFun != NULL && data->listenerObj != NULL )
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
}


static void __handleAccessoryParaGet(iOBiDiB bidib, int uid, byte* pdata ) {
  iOBiDiBData data = Data(bidib);
  iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( node, wProgram.acc_getparam );
  wProgram.setlntype(node, wProgram.lntype_bidib);
  wProgram.setmodid( node, uid );
  wProgram.setaddr( node, pdata[0] );
  wProgram.setcv( node, pdata[1] );
  wProgram.setval1( node, pdata[2] );

  if( pdata[1] == BIDIB_ACCESSORY_PARA_MACROMAP ) {
    wProgram.setval2( node, pdata[3] );
    wProgram.setval3( node, pdata[4] );
    wProgram.setval4( node, pdata[5] );
    wProgram.setval5( node, pdata[6] );
    wProgram.setval6( node, pdata[7] );
    wProgram.setval7( node, pdata[8] );
    wProgram.setval8( node, pdata[9] );
    wProgram.setval9( node, pdata[10] );
    wProgram.setval10( node, pdata[11] );
    wProgram.setval11( node, pdata[12] );
    wProgram.setval12( node, pdata[13] );
    wProgram.setval13( node, pdata[14] );
    wProgram.setval14( node, pdata[15] );
    wProgram.setval15( node, pdata[16] );
    wProgram.setval16( node, pdata[17] );
  }

  if( data->iid != NULL )
    wProgram.setiid( node, data->iid );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "MSG_ACCESSORY_PARA accessory %d: parameter=%d",  pdata[0], pdata[1] );

  if( pdata[1] == BIDIB_ACCESSORY_PARA_MACROMAP ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "macromap for accessory[%d] = %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
        pdata[0],
        pdata[2],pdata[3],pdata[4],pdata[5],pdata[6],pdata[7],pdata[8],pdata[9],
        pdata[10],pdata[11],pdata[12],pdata[13],pdata[14],pdata[15],pdata[16],pdata[17]);
  }

  if( data->listenerFun != NULL && data->listenerObj != NULL )
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
}


static void __handleMacroParaGet(iOBiDiB bidib, int uid, byte* pdata ) {
  iOBiDiBData data = Data(bidib);
  iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( node, wProgram.macro_getparams );
  wProgram.setlntype(node, wProgram.lntype_bidib);
  wProgram.setmodid( node, uid );
  wProgram.setval1( node, pdata[0] );
  wProgram.setval2( node, pdata[1] );
  wProgram.setval3( node, pdata[2] );
  wProgram.setval4( node, pdata[3] );
  wProgram.setval5( node, pdata[4] );
  wProgram.setval6( node, pdata[5] );
  if( data->iid != NULL )
    wProgram.setiid( node, data->iid );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "MSG_LC_MACRO_PARA macro %d: parameter=%d, valLSB=%d, valM1=%d, valM2=%d, valMSB=%d",
      pdata[0], pdata[1], pdata[2], pdata[3], pdata[4], pdata[5] );

  if( data->listenerFun != NULL && data->listenerObj != NULL )
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
}


static void __handleMacroStat(iOBiDiB bidib, int uid, byte* pdata ) {
  iOBiDiBData data = Data(bidib);
  iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  if( pdata[1] == BIDIB_MACRO_RESTORE )
    wProgram.setcmd( node, wProgram.macro_restore );
  else if( pdata[1] == BIDIB_MACRO_SAVE )
    wProgram.setcmd( node, wProgram.macro_save );
  else if( pdata[1] == BIDIB_MACRO_DELETE )
    wProgram.setcmd( node, wProgram.macro_delete );
  else {
    NodeOp.base.del(node);
    return;
  }
  wProgram.setlntype(node, wProgram.lntype_bidib);
  wProgram.setmodid( node, uid );
  wProgram.setval1( node, pdata[0] );
  wProgram.setval2( node, pdata[1] );
  if( data->iid != NULL )
    wProgram.setiid( node, data->iid );

  if( data->listenerFun != NULL && data->listenerObj != NULL )
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
}

static void __handleAccessory(iOBiDiB bidib, int uid, byte* pdata) {
  iOBiDiBData data = Data(bidib);
  iONode nodeC = NodeOp.inst( wAccessory.name(), NULL, ELEMENT_NODE );

  wAccessory.setnodenr( nodeC, uid );
  wAccessory.setdevid( nodeC, pdata[0]+1 );
  wAccessory.setval1( nodeC, pdata[1] );
  wAccessory.setval2( nodeC, pdata[2] );
  wAccessory.setval3( nodeC, pdata[3] );
  wAccessory.setval4( nodeC, pdata[4] );

  wAccessory.setaccevent( nodeC, True );
  if( data->iid != NULL )
    wAccessory.setiid( nodeC, data->iid );

  if( data->listenerFun != NULL && data->listenerObj != NULL )
    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
}


static void __handleAccessoryAck(iOBiDiB bidib, int uid, byte* pdata) {
  iOBiDiBData data = Data(bidib);
  int addr = pdata[0] + pdata[1]*256;
  int ack  = pdata[2];
  /* ToDo: How to handle? */
}


static void __handlePT(iOBiDiB bidib, int state, int val) {
  iOBiDiBData data = Data(bidib);

  iONode node = NULL;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT response %d:%d", state, val );
  if( state == 3 ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "cv %d has a value of %d", data->cv, val );
    data->value = val;
  }
  else if( state == 6 ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "cv %d is programmed to %d", data->cv, data->value );
  }
  else {
    return;
  }

  node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcv( node, data->cv );
  wProgram.setvalue( node, data->value );
  wProgram.setcmd( node, wProgram.datarsp );
  if( data->iid != NULL )
    wProgram.setiid( node, data->iid );

  if( data->listenerFun != NULL && data->listenerObj != NULL )
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );

}

static void __seqAck(iOBiDiB bidib, iOBiDiBNode bidibnode, byte Type, byte* pdata, int datasize) {
  iOBiDiBData data = Data(bidib);

  if( data->secAck && data->secAckInt > 0 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "seqAck for uid=%d", bidibnode->uid );
    TraceOp.dump ( name, TRCLEVEL_DEBUG, (char*)pdata, datasize );
    data->subWrite((obj)bidib, bidibnode->path, Type, pdata, datasize, bidibnode->seq++);
  }
}


static void __handleError(iOBiDiB bidib, byte* pdata) {
  iOBiDiBData data = Data(bidib);
  char txt[256] = {'\0'};

  switch( pdata[0] ) {
  case BIDIB_ERR_TXT: // Txt
    MemOp.set( txt, 0, 256 );
    MemOp.copy( txt, pdata+2, pdata[1]);
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "error message: [%s]", txt );
    break;
  case BIDIB_ERR_CRC: // CRC
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "CRC error in message number %d", pdata[1] );
    break;
  case BIDIB_ERR_SIZE: // Size
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Size error in message number %d", pdata[1] );
    break;
  case BIDIB_ERR_SEQUENCE: // Sequence
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Sequence error in message number %d; reset to zero", pdata[1] );
    __resetSeq(bidib);
    break;
  case BIDIB_ERR_PARAMETER: // Parameter
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Parameter error in message number %d", pdata[1] );
    break;
  case BIDIB_ERR_BUS: // Bus fault
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Bus fault: %d", pdata[1] );
    break;
  case BIDIB_ERR_ADDRSTACK:
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Address stack overflow: %02X%02X%02X%02X", pdata[1], pdata[2], pdata[3], pdata[4] );
    break;
  case BIDIB_ERR_IDDOUBLE:
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Double ID detected" );
    break;
  case BIDIB_ERR_SUBCRC:
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Sub node %d CRC error", pdata[1] );
    break;
  case BIDIB_ERR_SUBTIME:
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Sub node %d time out", pdata[1] );
    break;
  case BIDIB_ERR_HW: // Hardware error
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Hardware error: %d", pdata[1] );
    break;
  }
}


static Boolean __delNode(iOBiDiB bidib, byte* pdata) {
  iOBiDiBData data = Data(bidib);
  if( pdata != NULL ) {
    char uidKey[32];
    int uid = pdata[8] + (pdata[7] << 8) + (pdata[6] << 16) + (pdata[5] << 24);
    StrOp.fmtb( uidKey, "0x%08X", uid );
    iOBiDiBNode bidibnode = (iOBiDiBNode)MapOp.get( data->nodemap, uidKey );

    if( bidibnode != NULL ) {
      iONode node = __getIniNode(bidib, bidibnode->uid);
      char localKey[32];

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "removing node %s", uidKey);


      MapOp.remove( data->nodemap, uidKey );

      StrOp.fmtb( localKey, "%d.%d.%d.%d", bidibnode->path[0], bidibnode->path[1], bidibnode->path[2], bidibnode->path[3] );
      MapOp.remove( data->localmap, localKey);

      NodeOp.removeChild( data->bidibini, node );
    }
    return True;
  }
  return False;
}


static void __addNode(iOBiDiB bidib, byte* pdata) {
  iOBiDiBData data = Data(bidib);
  /*
  MSG_NODETAB:
  Es folgt ein Eintrag der Zuordnungstabelle, dieser hat folgenden Aufbau:
    MSG_NODETAB_DATA ::= NODETAB_VERSION NODETAB_ENTRY
    NODE_TAB_VERSION ::= [ 0x01 .. 0xff ] (Wird bei jeder Änderung inkrementiert, Überlauf: 255→1)
    NODETAB_ENTRY ::= NODE_ADDR UNIQUE_ID
  NODE_ADDR Zugewiesene lokale Adresse des Knotens (Wertebereich 0..127)
  UNIQUE_ID die eindeutige Hardwarekennung des Knotens, diese besteht aus 7 Bytes
  */
  //                                 UID
  // tab nodeaddr class res vid productid   crc
  // 01  00       40    00  0D  65 00 01 00 E1
  //  0   1        2     3   4   5  6  7  8

  // 0C 00 02 89 01 00 C0 00 0D 65 00 01 00 2F
  // pdata       01 00 C0 00 0D 65 00 01 00 2F
  // 0C 00 04 89 01 01 01 00 0D 6B 00 00 02 A0
  // pdata       01 01 01 00 0D 6B 00 00 02 A0
  char localKey[32];
  char uidKey[32];

  int uid = pdata[8] + (pdata[7] << 8) + (pdata[6] << 16) + (pdata[5] << 24);
  int classid = pdata[2];
  int vid = pdata[4];
  int localaddr = pdata[1];

  StrOp.fmtb( uidKey, "0x%08X", uid );

  //TraceOp.dump ( "bidibWrite", TRCLEVEL_INFO, (char*)pdata, 10 );

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
      "add node: class=0x%02X vid=%d uid=%s", classid, vid, uidKey);

  iOBiDiBNode node = (iOBiDiBNode)MapOp.get( data->nodemap, uidKey );

  if( node == NULL ) {
    char mnemonic[32] = {'\0'};
    byte l_msg[32];
    int size = 0;
    int msgidx = 0;
    char* classname = bidibGetClassName(classid, mnemonic);

    node = allocMem(sizeof(struct bidibnode));
    node->classid = classid;
    node->uid = uid;
    node->vendorid = vid;
    node->pendingfeature = -1;


    MemOp.copy(node->path+1, data->nodepath, 3);
    node->path[0] = localaddr;

    StrOp.fmtb( localKey, "%d.%d.%d.%d", node->path[0], node->path[1], node->path[2], node->path[3] );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "adding node: UID=[0x%08X] class=[%s] path=[%s]", uid, classname, localKey );

    MapOp.put( data->nodemap, uidKey, (obj)node );
    MapOp.put( data->localmap, localKey, (obj)node);

    data->subWrite((obj)bidib, node->path, MSG_SYS_GET_SW_VERSION, NULL, 0, 0);

    iONode child = NodeOp.inst(wBiDiBnode.name(), data->bidibini, ELEMENT_NODE);
    wBiDiBnode.setuid(child, uid);
    wBiDiBnode.setclass(child, classname);
    wBiDiBnode.setclassmnemonic(child, mnemonic);
    wBiDiBnode.setpath(child, localKey);
    wBiDiBnode.setvendor(child, node->vendorid);
    NodeOp.addChild(data->bidibini, child);
 
    if( data->defaultmain == NULL && StrOp.find(classname, wBiDiBnode.class_dcc_main) != NULL ) {
      data->defaultmain = node;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "setting node %s as default %s", uidKey, classname);
    }
    if( data->defaultbooster == NULL && StrOp.find(classname, wBiDiBnode.class_booster) != NULL ) {
      data->defaultbooster = node;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "setting node %s as default %s", uidKey, classname);
    }
    if( data->defaultprog == NULL && StrOp.find(classname, wBiDiBnode.class_dcc_prog) != NULL ) {
      data->defaultprog = node;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "setting node %s as default %s", uidKey, classname);
    }
    if( data->defaultswitch == NULL && StrOp.find(classname, wBiDiBnode.class_switch) != NULL ) {
      data->defaultswitch = node;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "setting node %s as default %s", uidKey, classname);
    }

    StrOp.free(classname);

  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "already registered: uid=%s", uidKey);
  }

}


static void __handleNodeFeature(iOBiDiB bidib, iOBiDiBNode bidibnode, byte Type, int size, byte* pdata, int datasize) {
  iOBiDiBData data = Data(bidib);

  if( bidibnode != NULL ) {
    if( Type == MSG_FEATURE_COUNT ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
          "MSG_FEATURE_COUNT, uid=%08X features=%d", bidibnode->uid, pdata[0] );
      data->subWrite((obj)bidib, bidibnode->path, MSG_FEATURE_GETNEXT, NULL, 0, bidibnode->seq++);
    }
    else if( Type == MSG_FEATURE ) {
      int feature = pdata[0];
      int value   = pdata[1];
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
          "MSG_FEATURE, uid=%08X feature=(%d) %s value=%d", bidibnode->uid, feature, bidibGetFeatureName(feature), value );

      if( feature == FEATURE_FW_UPDATE_MODE ) {
        bidibnode->fwup = (value ? True:False);
      }
      if( feature == FEATURE_BM_SIZE && !bidibnode->sod ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "setting sensor count to %d for %08X", value, bidibnode->uid );
        bidibnode->sensorcnt = value;
        if( bidibnode->pendingfeature == feature )
          bidibnode->pendingfeature = -1;
        __SoD(bidib, bidibnode);
      }
      else {
        iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
        wProgram.setcmd( node, wProgram.datarsp );
        wProgram.setiid( node, data->iid );
        wProgram.setlntype(node, wProgram.lntype_bidib);
        wProgram.setmodid(node, bidibnode->uid);
        wProgram.setmanu(node, bidibnode->vendorid);
        wProgram.setprod(node, bidibnode->classid);
        wProgram.setcv(node, feature);
        wProgram.setvalue(node, value);
        data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );

        if( bidibnode->pendingfeature == feature )
          bidibnode->pendingfeature = -1;
        else
          data->subWrite((obj)bidib, bidibnode->path, MSG_FEATURE_GETNEXT, NULL, 0, bidibnode->seq++);
      }
    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unhandled node feature: unknown node");
  }

}


static void __reportState(iOBiDiB bidib, iOBiDiBNode bidibnode, Boolean shortcut, Boolean power) {
  iOBiDiBData data = Data(bidib);
  iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
  if( data->iid != NULL )
    wState.setiid( node, data->iid );

  if( bidibnode != NULL ) {
    wState.setpower( node, ((bidibnode->stat & BIDIB_BST_STATE_ON)|power) ? True:False);
    wState.setload( node, bidibnode->load );
    wState.setvolt( node, bidibnode->volt );
    wState.settemp( node, bidibnode->temp );
    wState.setloadmax( node, bidibnode->loadmax );
    wState.setvoltmin( node, bidibnode->voltmin );
    wState.settempmax( node, bidibnode->tempmax );
    wState.setuid( node, bidibnode->uid);
  }
  else {
    wState.setpower( node, data->power );
    wState.setload( node, data->load );
    wState.setvolt( node, data->volt );
    wState.settemp( node, data->temp );
  }
  wState.settrackbus( node, data->power );
  wState.setsensorbus( node, True );
  wState.setaccessorybus( node, True );
  wState.setshortcut(node, shortcut);
  if( data->listenerFun != NULL && data->listenerObj != NULL )
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
}

static void __handleCSDriveAck(iOBiDiB bidib, iOBiDiBNode bidibnode, byte* pdata) {
  iOBiDiBData data = Data(bidib);
  int addr = pdata[0] + pdata[1] * 256;
  int ack = pdata[2];
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco %d ack=%d", addr, ack );
}


static const char* __csstate2str(int state, int* level) {
  *level = TRCLEVEL_MONITOR;
  switch( state ) {
  case BIDIB_CS_STATE_OFF:
    return "track power is off";
  case BIDIB_CS_STATE_STOP:
    *level = TRCLEVEL_WARNING;
    return "emergency break";
  case BIDIB_CS_STATE_SOFTSTOP:
    return "soft break";
  case BIDIB_CS_STATE_GO:
    return "track power is on";
  case BIDIB_CS_STATE_PROG:
    return "programming mode";
  case BIDIB_CS_STATE_PROGBUSY:
    return "programming pending";
  case BIDIB_CS_STATE_BUSY:
    *level = TRCLEVEL_WARNING;
    return "busy";
  }
  return "";
}

static void __handleCSStat(iOBiDiB bidib, iOBiDiBNode bidibnode, byte* pdata) {
  iOBiDiBData data = Data(bidib);
  int level = TRCLEVEL_MONITOR;

  /*
    0x00  BIDIB_CS_STATE_OFF  Die Gleisausgabe wird abgeschaltet.
    0x01  BIDIB_CS_STATE_STOP Alle Loks werden mittels Nothalt angehalten, jedoch Weichen können nach wie vor geschaltet werden.
          Wenn Stop von der Zentrale nicht unterstützt wird, so wird OFF ausgeführt.
    0x02  BIDIB_CS_STATE_SOFTSTOP Alle Loks werden mit Fahrstufe 0 (also mit ihrer eigenen Verzögerung) angehalten,
          Weichen können weiterhin geschaltet werden. Wenn Soft-Stop von der Zentrale nicht unterstützt wird,
          so wird STOP ausgeführt.
    0x03  BIDIB_CS_STATE_SHORT  Die Zentrale hat einen Fehler am Ausgang erkannt. Diese Nachricht ist nur relevant für
          Zentralen mit eigener Ausgang.
    0x10  BIDIB_CS_STATE_GO Wiederaufnahme des Betriebes, Loks und Weichen können geschaltet werden.
    0x80  BIDIB_CS_STATE_PROG Programmiermode; Die Zentrale hat in den Programmiermode umgeschaltet und ist zur Ausführung von
          Programmierbefehlen (auf den Programmiergleis) bereit. Der normale Betrieb ruht.
    0x81  BIDIB_CS_STATE_PROGBUSY Programmiermode; diese Meldung zeigt an, dass aktuell ein Programmiervorgang auf dem
          Programmiergleis durchgeführt wird. (nur bei Abfrage)
    0xF0  BIDIB_CS_STATE_BUSY Die Gleisausgabe ist nicht mehr aufnahmefähig für neue Befehle, z.B. weil entsprechende
          Ausgabe-Fifos voll sind. (nur bei Abfrage oder beim Senden einer MSG_CS_DRIVE )
   */

  TraceOp.trc( name, level, __LINE__, 9999, "CS state=0x%02X [%s][%08X]", pdata[0], __csstate2str(pdata[0], &level), bidibnode!=NULL?bidibnode->uid:0 );
  data->power = (pdata[0] == BIDIB_CS_STATE_OFF) ? False:True;
  if( bidibnode != NULL ) {
    bidibnode->stat &= ~BIDIB_BST_STATE_ON;
    bidibnode->stat |= data->power?BIDIB_BST_STATE_ON:0;
  }
  __reportState(bidib, bidibnode, False, data->power);
}


static const char* __boosterState2Str(int state, int* level, Boolean* shortcut) {
  *level = TRCLEVEL_MONITOR;
  switch( state ) {
  case BIDIB_BST_STATE_OFF:
    return "power off";
  case BIDIB_BST_STATE_OFF_SHORT:
    *level = TRCLEVEL_EXCEPTION;
    *shortcut = True;
    return "overload";
  case BIDIB_BST_STATE_OFF_HOT:
    *level = TRCLEVEL_EXCEPTION;
    return "over temperature";
  case BIDIB_BST_STATE_OFF_NOPOWER:
    *level = TRCLEVEL_WARNING;
    return "no power supply";
  case BIDIB_BST_STATE_OFF_GO_REQ:
    return "pending power on";
  case BIDIB_BST_STATE_OFF_HERE:
    return "manual power off";
  case BIDIB_BST_STATE_OFF_NO_DCC:
    return "DCC missing";
  case BIDIB_BST_STATE_ON:
    *shortcut = False;
    return "power on";
  case BIDIB_BST_STATE_ON_LIMIT:
    *level = TRCLEVEL_WARNING;
    return "reducing current!";
  case BIDIB_BST_STATE_ON_HOT:
    *level = TRCLEVEL_WARNING;
    return "running hot!";
  case BIDIB_BST_STATE_ON_STOP_REQ:
    return "pending power off";
  case BIDIB_BST_STATE_ON_HERE:
    return "manual power on";
  }
  return "";
}

static void __handleBoosterStat(iOBiDiB bidib, iOBiDiBNode bidibnode, byte* pdata) {
  iOBiDiBData data = Data(bidib);
  int level = TRCLEVEL_MONITOR;
  Boolean shortcut = False;
  /*
      Wert  Alias Bedeutung
      0x00  BIDIB_BST_STATE_OFF Booster ist abgeschaltet (allgemein, z.B. auf Grund Host-Befehl).
      0x01  BIDIB_BST_STATE_OFF_SHORT Booster ist abgeschaltet (wegen Kurzschluß).
      0x02  BIDIB_BST_STATE_OFF_HOT Booster ist abgeschaltet (wegen Übertemperatur).
      0x03  BIDIB_BST_STATE_OFF_NOPOWER Booster ist abgeschaltet (wegen fehlender Netzspannung).
      0x04  BIDIB_BST_STATE_OFF_GO_REQ  Booster ist abgeschaltet und es liegt eine Einschalt-Anforderung vor.
      0x05  BIDIB_BST_STATE_OFF_HERE  Booster ist abgeschaltet (auf Grund lokalem Tastendruck).
      0x06  BIDIB_BST_STATE_OFF_NO_DCC  Booster ist abgeschaltet (wegen fehlendem Eingangssignal).
      0x80  BIDIB_BST_STATE_ON  Booster ist eingeschaltet.
      0x81  BIDIB_BST_STATE_ON_LIMIT  Booster ist eingeschaltet und läuft in der Strombegrenzung.
      0x82  BIDIB_BST_STATE_ON_HOT  Booster ist eingeschaltet und ist im kritischen Temperaturbereich.
      0x83  BIDIB_BST_STATE_ON_STOP_REQ Booster ist eingeschaltet und es liegt eine Stop-Anforderung vor.
      0x84  BIDIB_BST_STATE_ON_HERE Booster ist eingeschaltet (auf Grund lokalem Tastendruck).
  */
  if( bidibnode != NULL ) {
    const char* msg = __boosterState2Str(pdata[0], &level, &bidibnode->shortcut);
    bidibnode->stat = pdata[0];
    TraceOp.trc( name, level, __LINE__, 9999,
        "booster %08X state=0x%02X [%s]", bidibnode->uid, pdata[0], msg );
    shortcut = bidibnode->shortcut;
  }
  else {
    TraceOp.trc( name, level, __LINE__, 9999,
        "booster state=0x%02X [%s]", pdata[0], __boosterState2Str(pdata[0], &level, &shortcut) );
  }
  data->power = (pdata[0] & 0x80) ? True:False;
  __reportState(bidib, bidibnode, shortcut, data->power);
}


static int __calcCurrent(int current) {
  /*
    0 No current consumption, track is free.
    1..15 Current consumption, in mA. Possible range: 1..15mA
    16..63  Current consumption, the value is (date - 12) * 4mA. Possible range: 16..204mA
    64..127 Current consumption, the value is (date - 51) * 16mA. Possible range: 208..1216mA
    128..191  Current consumption, the value is (date - 108) * 64mA. Possible range: 1280..5312mA
    192..250  Current consumption, the value is (date - 171) * 256mA. Possible range: 5376..20224mA
    251..253  Reserved.
    254 Overcurrent occured.
    0xFF  No exact consumption value known.
   */
  if( current > 15 && current < 64 )
    current = (current - 12) * 4;
  else if( current > 63 && current < 128 )
    current = (current - 51) * 16;
  else if( current > 127 && current < 192 )
    current = (current - 108) * 64;
  else if( current > 191 && current < 251 )
    current = (current - 171) * 256;
  else if( current > 250 )
    current = 0;

  return current;
}

static void __handleBoosterDiagnostic(iOBiDiB bidib, iOBiDiBNode bidibnode, byte* pdata, int datasize) {
  iOBiDiBData data = Data(bidib);
  int uid     = 0;
  int current = 0;
  int volt    = 0;
  int temp    = 0;
  int i       = 0;

  for( i = 0; i < datasize; i+=2 ) {
    switch( pdata[i] ) {
    case BIDIB_BST_DIAG_I:
      current = pdata[i+1];
      break;
    case BIDIB_BST_DIAG_V:
      volt = pdata[i+1] * 100; /* units=100mV */
      break;
    case BIDIB_BST_DIAG_T:
      temp = pdata[i+1];
      break;
    }
  }

  current = __calcCurrent(current);
  if( current > 5000 ) {
    /* reject value... */
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unexpected booster load=%dmA", current );
    current = bidibnode->load;
  }

  if( bidibnode != NULL && (bidibnode->load != current || bidibnode->volt != volt || bidibnode->temp != temp) ) {
    uid = bidibnode->uid;
    bidibnode->load = current;
    if( current > bidibnode->loadmax )
      bidibnode->loadmax = current;
    bidibnode->volt = volt;
    if( bidibnode->voltmin > volt || bidibnode->voltmin == 0)
      bidibnode->voltmin = volt;
    bidibnode->temp = temp;
    if( temp > bidibnode->tempmax )
      bidibnode->tempmax = temp;
    data->load = current;
    data->volt = volt;
    data->temp = temp;
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "booster %08X load=%dmA %dmV %d°C", bidibnode->uid, current, volt, temp );
    __reportState(bidib, bidibnode, False, False);
  }
  else if( data->load != current ) {
    data->load = current;
    data->volt = volt;
    data->temp = temp;
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "booster load=%dmA %dmV %d°C", current, volt, temp );
    __reportState(bidib, NULL, False, False);
  }
}


static void __handleBoosterCurrent(iOBiDiB bidib, iOBiDiBNode bidibnode, byte* pdata) {
  iOBiDiBData data = Data(bidib);
  int current = pdata[0];
  int uid = 0;

  current = __calcCurrent(current);

  if( bidibnode != NULL) {
    if(bidibnode->load != current) {
      uid = bidibnode->uid;
      bidibnode->load = current;
      if( bidibnode->loadmax < current )
        bidibnode->loadmax = current;
      data->load = current;
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "booster %08X load=%d mA", bidibnode->uid, data->load );
      __reportState(bidib, bidibnode, False, False);
    }
  }
  else if( data->load != current ) {
    data->load = current;
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "booster load=%d mA", data->load );
    __reportState(bidib, NULL, False, False);
  }
}


static void __handleConfig(iOBiDiB bidib, iOBiDiBNode bidibnode, byte* pdata) {
  iOBiDiBData data = Data(bidib);
  int feature = pdata[0];
  int value   = pdata[1];
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
      "MSG_LC_CONFIG, uid=%08X type=%d port=%d val1=%d val2=%d val3=%d val4=%d",
      bidibnode->uid, pdata[0], pdata[1], pdata[2], pdata[3], pdata[4], pdata[5]);

  if( bidibnode != NULL ) {
    iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setcmd( node, wProgram.nvget );
    wProgram.setiid( node, data->iid );
    wProgram.setlntype(node, wProgram.lntype_bidib);
    wProgram.setporttype(node, pdata[0]);
    wProgram.setmodid(node, bidibnode->uid);
    wProgram.setcv(node, pdata[1]);
    wProgram.setval1(node, pdata[2]);
    wProgram.setval2(node, pdata[3]);
    wProgram.setval3(node, pdata[4]);
    wProgram.setval4(node, pdata[5]);
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
}

static void __handleNodeTab(iOBiDiB bidib, iOBiDiBNode node, int Type, const char* pathKey, byte* pdata, int datasize) {
  iOBiDiBData data = Data(bidib);
  byte path[4] = {0,0,0,0}; // Default path in case no node was found.

  //                                 UID
  //             ver len start locaddr class res vid productid   crc
  // 0E 00 04 89 01  01  00    00      40    00  0D  65 00 01 00 E1
  int entry    = 0;
  int offset   = 7;


  if( Type == MSG_NODETAB_COUNT ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "MSG_NODETAB_COUNT, path=%s count=%d", pathKey, pdata[0] );
    if( pdata[0] == 0 ) {
      // bus is in reset modus...
      ThreadOp.sleep(100);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "retry get node table...");
      data->subWrite((obj)bidib, node==NULL?path:node->path, MSG_NODETAB_GETALL, NULL, 0, 0);
    }
    else {
      // request next
      data->subWrite((obj)bidib, node==NULL?path:node->path, MSG_NODETAB_GETNEXT, NULL, 0, node==NULL?0:node->seq++);
    }
    return;
  }
  else if( Type == MSG_NODETAB ) {
    data->tabver = pdata[0];
    int entry  = pdata[1];
    byte l_msg[32];

    /*
    00000000: 0C 00 07 89 01 00 40 00 0D 66 00 00 01 00
    20120519.082709.379 r9999c bidibrea OBiDiB   0779 MSG_NODETAB, addr=0 seq=7 tab-ver=1 tab-len=0

    MSG_NODETAB:
    Es folgt ein Eintrag der Zuordnungstabelle, dieser hat folgenden Aufbau:
      MSG_NODETAB_DATA ::= NODETAB_VERSION NODETAB_ENTRY
      NODE_TAB_VERSION ::= [ 0x01 .. 0xff ] (Wird bei jeder Änderung inkrementiert, Überlauf: 255→1)
      NODETAB_ENTRY ::= NODE_ADDR UNIQUE_ID

    NODE_ADDR Zugewiesene lokale Adresse des Melders (Wertebereich 0..127)
    UNIQUE_ID die eindeutige Hardwarekennung des Melders, diese besteht aus 7 Bytes
    */

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "MSG_NODETAB, path=%s tab-ver=%d tab-idx=%d", pathKey, data->tabver, entry );
    // ToDo: Data offset in TAB message.
    __addNode(bidib, pdata );

    data->subWrite((obj)bidib, node==NULL?path:node->path, MSG_NODETAB_GETNEXT, NULL, 0, node==NULL?0:node->seq++);
  }

    /*

    char localKey[32];
    char uidKey[32];
    int uid = msg[offset+4+entry*8] + (msg[offset+5+entry*8] << 8) + (msg[offset+6+entry*8] << 16) + (msg[offset+7+entry*8] << 24);
    StrOp.fmtb( localKey, "%d", msg[offset+0+entry*8] );
    StrOp.fmtb( uidKey, "%d", uid );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "entry=%d local=%s class=0x%02X vid=%d uid=%s", entry,
        localKey, msg[offset+1+entry*8], msg[offset+3+entry*8], uidKey);

    iONode node = (iONode)MapOp.get( data->nodemap, uidKey );
    if( node != NULL ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "mapping product ID [%s] with local [%s] to offset [%d]", uidKey, localKey, wBiDiBnode.getoffset(node) );
      MapOp.put( data->localmap, localKey, (obj)node);
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "no mapping found for product ID [%s]", uidKey );
    }
    */
}


static void __handleNewNode(iOBiDiB bidib, iOBiDiBNode bidibnode, byte* pdata, int size) {
  iOBiDiBData data = Data(bidib);
  if( bidibnode != NULL && pdata != NULL ) {
    data->tabver = pdata[0];
    __addNode(bidib, pdata);
    data->subWrite((obj)bidib, bidibnode->path, MSG_NODE_CHANGED_ACK, pdata, 1, bidibnode->seq++);
    ThreadOp.sleep(50);
    data->subWrite((obj)bidib, bidibnode->path, MSG_SYS_ENABLE, NULL, 0, bidibnode->seq++);

  }
}


static void __handleLostNode(iOBiDiB bidib, iOBiDiBNode bidibnode, byte* pdata, int size) {
  iOBiDiBData data = Data(bidib);
  TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "MSG_NODE_LOST" );
  if( __delNode(bidib, pdata) && bidibnode != NULL) {
    data->subWrite((obj)bidib, bidibnode->path, MSG_NODE_CHANGED_ACK, pdata, 1, bidibnode->seq++);
    {
      iONode cmd = NodeOp.inst(wSysCmd.name(), NULL, ELEMENT_NODE);
      wSysCmd.setcmd(cmd, wSysCmd.stop);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "power off on lost node..." );
      BiDiBOp.cmd((obj)bidib, cmd);
    }
  }
}


static void __handleStat(iOBiDiB bidib, iOBiDiBNode bidibnode, byte* pdata) {
  iOBiDiBData data = Data(bidib);
  // Report it to the server.
  if( bidibnode != NULL ) {
    iONode nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );

    wSwitch.setbus( nodeC, bidibnode->uid );
    wSwitch.setaddr1( nodeC, pdata[1]+1 );

    if( data->iid != NULL )
      wSwitch.setiid( nodeC, data->iid );

    if( pdata[0] == BIDIB_OUTTYPE_SERVO )
      wSwitch.setsinglegate(nodeC, True);

    wSwitch.setstate( nodeC, pdata[2]?wSwitch.turnout:wSwitch.straight );
    wSwitch.setgatevalue(nodeC, pdata[2]);
    wSwitch.setporttype(nodeC, pdata[0]);

    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
  }
}

static void __handleIdentify(iOBiDiB bidib, iOBiDiBNode bidibnode, const char* path) {
  iOBiDiBData data = Data(bidib);
  /* Notify server. */
  if( bidibnode != NULL ) {
    iONode child = __getIniNode(bidib, bidibnode->uid);
    iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setcmd( node, wProgram.type );
    wProgram.setiid( node, data->iid );
    wProgram.setlntype(node, wProgram.lntype_bidib);
    wProgram.setmodid(node, bidibnode->uid);
    wProgram.setmanu(node, bidibnode->vendorid);
    wProgram.setprod(node, bidibnode->classid);
    if( child != NULL ) {
      wProgram.setstrval1(node, wBiDiBnode.getversion(child) );
    }
    wProgram.setfilename(node, path);
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,"ident path=%s uid=%08X, vendor=%d class=0x%02X",
        path, bidibnode->uid, bidibnode->vendorid, bidibnode->classid);
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
}


static void __handleUpdateStat(iOBiDiB bidib, iOBiDiBNode bidibnode, byte* pdata) {
  iOBiDiBData data = Data(bidib);
  char msgdata[256];

  if( bidibnode != NULL ) {
    if( pdata[0] == BIDIB_MSG_FW_UPDATE_STAT_READY ) {
      if( data->hexstate == 1 ) {
        // lightcontrol_v0.10.05.001.hex
        const char* dest = StrOp.findi(data->hexfile, ".hex");
        if( dest != NULL ) {
          msgdata[0] = BIDIB_MSG_FW_UPDATE_OP_SETDEST;
          msgdata[1] = atoi(dest-3); /* dest memory: name_version.ddd.hex */
          bidibnode->seq = 0;
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,"BIDIB_MSG_FW_UPDATE_OP_SETDEST destination=%d", msgdata[1]);
          data->subWrite((obj)bidib, bidibnode->path, MSG_FW_UPDATE_OP, msgdata, 2, 0);
        }
      }
      if( data->hexstate == 2 ) {
        iONode rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
        data->hexstate = 0;
        msgdata[0] = BIDIB_MSG_FW_UPDATE_OP_EXIT;
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "node is ready; exit update...");
        data->subWrite((obj)bidib, bidibnode->path, MSG_FW_UPDATE_OP, msgdata, 1, 0);

        wProgram.setcmd( rsp, wProgram.writehex );
        wProgram.setlntype(rsp, wProgram.lntype_bidib);
        wProgram.setrc( rsp, wProgram.rc_ok );
        wProgram.setmodid( rsp, bidibnode->uid );
        if( data->iid != NULL )
          wProgram.setiid( rsp, data->iid );
        if( data->listenerFun != NULL && data->listenerObj != NULL )
          data->listenerFun( data->listenerObj, rsp, TRCLEVEL_INFO );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "node is ready but no update is pending...");
      }
    }
    else if(pdata[0] == BIDIB_MSG_FW_UPDATE_STAT_DATA) {
      if( pdata[1] > 0 ) {
        ThreadOp.sleep(pdata[1]*10);
      }
      // Send a HEX line
      if( data->hexline == 0 ) {
        data->hexfh = FileOp.inst( data->hexfile, OPEN_READONLY );
      }

      if( data->hexfh != NULL ) {
        FILE* fs = FileOp.getStream(data->hexfh);
        fgets( msgdata+1, 256, fs );
        if( !ferror(fs) && !feof(fs) ) {
          StrOp.replaceAll(msgdata+1, '\r', '\0');
          StrOp.replaceAll(msgdata+1, '\n', '\0');
          TraceOp.trc( "bidib", TRCLEVEL_INFO, __LINE__, 9999, "update line=%d [%s]", data->hexline++, msgdata+1);

          msgdata[0] = BIDIB_MSG_FW_UPDATE_OP_DATA;
          data->subWrite((obj)bidib, bidibnode->path, MSG_FW_UPDATE_OP, msgdata, 1 + StrOp.len(msgdata+1), 0);
          if( data->hexline % 10 == 0 ) {
            iONode rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
            if( data->iid != NULL )
              wProgram.setiid( rsp, data->iid );
            wProgram.setcmd( rsp, wProgram.writehex );
            wProgram.setrc( rsp, wProgram.rc_ok );
            wProgram.setrs( rsp, data->hexline );
            wProgram.setmodid( rsp, bidibnode->uid );
            wProgram.setlntype(rsp, wProgram.lntype_bidib);
            if( data->listenerFun != NULL && data->listenerObj != NULL )
              data->listenerFun( data->listenerObj, rsp, TRCLEVEL_INFO );
          }
        }
        else {
          // Last line processed? BIDIB_MSG_FW_UPDATE_OP_DONE
          TraceOp.trc( "bidib", TRCLEVEL_INFO, __LINE__, 9999, "all lines processed: BIDIB_MSG_FW_UPDATE_OP_DONE");
          FileOp.base.del( data->hexfh );
          data->hexfh = NULL;
          data->hexstate = 2;
          msgdata[0] = BIDIB_MSG_FW_UPDATE_OP_DONE;
          data->subWrite((obj)bidib, bidibnode->path, MSG_FW_UPDATE_OP, msgdata, 1, 0);
        }
      }

    }
    else if(pdata[0] == BIDIB_MSG_FW_UPDATE_STAT_EXIT) {
      iONode rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,"BIDIB_MSG_FW_UPDATE_STAT_EXIT from node %08X", bidibnode->uid);
      wProgram.setcmd( rsp, wProgram.writehex );
      wProgram.setlntype(rsp, wProgram.lntype_bidib);
      wProgram.setrc( rsp, wProgram.rc_ok );
      wProgram.setmodid( rsp, bidibnode->uid );
      if( data->iid != NULL )
        wProgram.setiid( rsp, data->iid );
      if( data->listenerFun != NULL && data->listenerObj != NULL )
        data->listenerFun( data->listenerObj, rsp, TRCLEVEL_INFO );
    }
    else if(pdata[0] == BIDIB_MSG_FW_UPDATE_STAT_ERROR) {
      iONode rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,"MSG_FW_UPDATE_STAT error=%d", pdata[1]);
      if( data->hexfh != NULL )
        FileOp.base.del( data->hexfh );
      data->hexfh = NULL;
      data->hexstate = 0;
      wProgram.setcmd( rsp, wProgram.writehex );
      wProgram.setlntype(rsp, wProgram.lntype_bidib);
      wProgram.setrc( rsp, wProgram.rc_error );
      wProgram.setrs( rsp, pdata[1] );
      wProgram.setmodid( rsp, bidibnode->uid );
      if( data->iid != NULL )
        wProgram.setiid( rsp, data->iid );
      if( data->listenerFun != NULL && data->listenerObj != NULL )
        data->listenerFun( data->listenerObj, rsp, TRCLEVEL_INFO );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,"MSG_FW_UPDATE_STAT status=%d ???", pdata[0]);
    }
  }
}


static void __handleDriveManual(iOBiDiB bidib, int uid, byte* pdata) {
  iOBiDiBData data = Data(bidib);
  /*
    ADDR_L  Adresse, untere 8 Bits
    ADDR_H  Adresse, obere 8 Bits
    data[0] Bitfeld, Format
    data[1] Bitfeld Ausgabe Aktiv
    data[2] Geschwindigkeit
    data[3] 3*reserved, FL (Licht), F4, F3, F2, F1
    data[4] F12 - F5
    data[5] F20 - F13
    data[6] F28 - F21
  */
  int change = pdata[3];
  int addr = pdata[0] + pdata[1] * 256;
  int speed = pdata[4] & 0x7F;
  Boolean dir = (pdata[4] & 0x80) ? True:False;
  iOSlot slot = __getSlotByAddr(bidib, addr);

  if( change & 0x01 ) {
    /* speed */
    iONode nodeC = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    if( data->iid != NULL )
      wLoc.setiid( nodeC, data->iid );
    if( slot != NULL )
      wLoc.setid( nodeC, slot->id );

    wLoc.setaddr( nodeC, addr );
    wLoc.setV_raw( nodeC, speed );
    wLoc.setdir( nodeC, dir );
    wLoc.setcmd( nodeC, wLoc.velocity );
    wLoc.setthrottleid( nodeC, "bidib" );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "loco=%s addr=%d speed=%d dir=%s", slot!=NULL?slot->id:"-", addr, speed, dir?"fwd":"rev" );

    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
  }

  if( change & 0x3E ) {
    /* function group */
    iONode nodeD = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
    if( data->iid != NULL )
      wLoc.setiid( nodeD, data->iid );
    if( slot != NULL )
      wFunCmd.setid( nodeD, slot->id );
    wFunCmd.setaddr( nodeD, addr );
    wLoc.setthrottleid( nodeD, "bidib" );

    if( change & 0x02 ) {
      int fg1 = pdata[5];
      wLoc.setfn( nodeD, (fg1 & 0x10) ? True:False );
      wFunCmd.setf0( nodeD, (fg1 & 0x10) ? True:False );
      wFunCmd.setf1( nodeD, (fg1 & 0x01) ? True:False );
      wFunCmd.setf2( nodeD, (fg1 & 0x02) ? True:False );
      wFunCmd.setf3( nodeD, (fg1 & 0x04) ? True:False );
      wFunCmd.setf4( nodeD, (fg1 & 0x08) ? True:False );
      wFunCmd.setgroup( nodeD, 1 );
      data->listenerFun( data->listenerObj, nodeD, TRCLEVEL_INFO );
    }

    if( change & 0x04 ) {
      int fg2 = pdata[6];
      wFunCmd.setf5( nodeD, (fg2 & 0x01) ? True:False );
      wFunCmd.setf6( nodeD, (fg2 & 0x02) ? True:False );
      wFunCmd.setf7( nodeD, (fg2 & 0x04) ? True:False );
      wFunCmd.setf8( nodeD, (fg2 & 0x08) ? True:False );
      wFunCmd.setgroup( nodeD, 2 );
      data->listenerFun( data->listenerObj, nodeD, TRCLEVEL_INFO );
    }

    if( change & 0x08 ) {
      int fg3 = pdata[6];
      wFunCmd.setf9 ( nodeD, (fg3 & 0x10) ? True:False );
      wFunCmd.setf10( nodeD, (fg3 & 0x20) ? True:False );
      wFunCmd.setf11( nodeD, (fg3 & 0x40) ? True:False );
      wFunCmd.setf12( nodeD, (fg3 & 0x80) ? True:False );
      wFunCmd.setgroup( nodeD, 3 );
      data->listenerFun( data->listenerObj, nodeD, TRCLEVEL_INFO );
    }

    if( change & 0x10 ) {
      int fg = pdata[7];
      wFunCmd.setf13( nodeD, (fg & 0x01) ? True:False );
      wFunCmd.setf14( nodeD, (fg & 0x02) ? True:False );
      wFunCmd.setf15( nodeD, (fg & 0x04) ? True:False );
      wFunCmd.setf16( nodeD, (fg & 0x08) ? True:False );
      wFunCmd.setf17( nodeD, (fg & 0x10) ? True:False );
      wFunCmd.setf18( nodeD, (fg & 0x20) ? True:False );
      wFunCmd.setf19( nodeD, (fg & 0x40) ? True:False );
      wFunCmd.setf20( nodeD, (fg & 0x80) ? True:False );
      wFunCmd.setgroup( nodeD, 4 );
      data->listenerFun( data->listenerObj, (iONode)NodeOp.base.clone(nodeD), TRCLEVEL_INFO );
      wFunCmd.setgroup( nodeD, 5 );
      data->listenerFun( data->listenerObj, nodeD, TRCLEVEL_INFO );
    }

    if( change & 0x20 ) {
      int fg = pdata[8];
      wFunCmd.setf21( nodeD, (fg & 0x01) ? True:False );
      wFunCmd.setf22( nodeD, (fg & 0x02) ? True:False );
      wFunCmd.setf23( nodeD, (fg & 0x04) ? True:False );
      wFunCmd.setf24( nodeD, (fg & 0x08) ? True:False );
      wFunCmd.setf25( nodeD, (fg & 0x10) ? True:False );
      wFunCmd.setf26( nodeD, (fg & 0x20) ? True:False );
      wFunCmd.setf27( nodeD, (fg & 0x40) ? True:False );
      wFunCmd.setf28( nodeD, (fg & 0x80) ? True:False );
      wFunCmd.setgroup( nodeD, 6 );
      data->listenerFun( data->listenerObj, (iONode)NodeOp.base.clone(nodeD), TRCLEVEL_INFO );
      wFunCmd.setgroup( nodeD, 7 );
      data->listenerFun( data->listenerObj, nodeD, TRCLEVEL_INFO );
    }

  }
}



/**
 * len addr seq type data  crc
 * 05  00   00  81   FE AF 89
 */
static Boolean __processBidiMsg(iOBiDiB bidib, byte* msg, int size) {
  iOBiDiBData data = Data(bidib);
  iOBiDiBNode bidibnode = NULL;
  byte path[4] = {0,0,0,0};
  int pathidx = 0;
  char pathKey[32] = {'\0'};
  byte msgdata[127];
  byte* pdata = NULL;

  /*                  pathidx: 0  1  2  3
   * Addr can be 4 bytes long. XX XX XX XX */
  for( pathidx = 0; pathidx < 4; pathidx++ ) {
    path[pathidx] = msg[1+pathidx];
    if( path[pathidx] == 0 )
      break;
  }
  int  Seq = msg[2+pathidx];
  int Type = msg[3+pathidx];
  pdata = msg+4+pathidx;
  int datasize = size - (4 + pathidx);

  StrOp.fmtb( pathKey, "%d.%d.%d.%d", path[0], path[1], path[2], path[3] );

  bidibnode = (iOBiDiBNode)MapOp.get( data->localmap, pathKey );

  if( bidibnode == NULL ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "node not found by local address [%s]", pathKey );
  }

  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999,
      "processing bidib message path=%s type=0x%02X uid=0x%08X", pathKey, Type, bidibnode!=NULL?bidibnode->uid:0 );

  switch( Type ) {
  case MSG_SYS_MAGIC:
  { // len = 5
    int Magic = (pdata[1]<<8)+pdata[0];
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "MSG_SYS_MAGIC, path=%s seq=%d magic=0x%04X", pathKey, Seq, Magic );
    data->upSeq   = Seq;
    data->magicOK = True;

    /* Clean up node list. */
    iONode child = wBiDiB.getbidibnode(data->bidibini);
    while( child != NULL ) {
      NodeOp.removeChild(data->bidibini, child);
      child = wBiDiB.getbidibnode(data->bidibini);
    }

    // start getting the node table from the PC interface
    data->nodepath[0] = 0;
    path[0] = 0; // address

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "reset bus...");
    data->subWrite((obj)bidib, path, MSG_SYS_RESET, NULL, 0, 0);
    ThreadOp.sleep(1000);

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "get node table...");
    data->subWrite((obj)bidib, path, MSG_NODETAB_GETALL, NULL, 0, 0);

    break;
  }

  case MSG_SYS_SW_VERSION:
  { // len = 6
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "MSG_SYS_SW_VERSION, path=%s version=%d.%d.%d", pathKey, pdata[2], pdata[1], pdata[0] );

    if( bidibnode != NULL ) {
      iONode child = __getIniNode(bidib, bidibnode->uid);
      if( child != NULL ) {
        char ver[32];
        StrOp.fmtb( ver, "%d.%d.%d", pdata[2], pdata[1], pdata[0] );
        wBiDiBnode.setversion(child, ver);
        if( pdata[2]==0 && pdata[1]==0 && pdata[0]==0 ) {
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,"uid=%d bootloader only; No application loaded.", bidibnode->uid);
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,"no ini node found for uid=%d", bidibnode->uid);
      }
    }

    break;
  }

  case MSG_NODETAB_COUNT:
  case MSG_NODETAB:
  {
    __handleNodeTab(bidib, bidibnode, Type, pathKey, pdata, datasize);
    break;
  }

  case MSG_NODE_NEW:
  {
    __handleNewNode(bidib, bidibnode, pdata, datasize);
    break;
  }

  case MSG_NODE_LOST:
  {
    __handleLostNode(bidib, bidibnode, pdata, datasize);
    break;
  }

  /*
   * 04 00 02 A0 00 BE MSG_BM_OCC
   * 04 00 03 A0 10 88
   * 04 00 04 A1 00 AB MSG_BM_FREE
   * 04 00 05 A0 10 59
   */
  case MSG_BM_OCC:
  { // len = 4
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,"BM port %d occupied", pdata[0]);
    if(!bidibnode->occ[pdata[0]]) {
      bidibnode->occ[pdata[0]] = True;
      __handleSensor(bidib, bidibnode->uid, pdata[0], True, 0, -1, bidibnode->bmload[pdata[0]]);
    }
    __seqAck(bidib, bidibnode, MSG_BM_MIRROR_OCC, pdata, datasize);
    break;
  }

  case MSG_BM_FREE:
  { // len = 4
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,"BM port %d free", pdata[0]);
    bidibnode->occ[pdata[0]] = False;
    __handleSensor(bidib, bidibnode->uid, pdata[0], False, 0, -1, bidibnode->bmload[pdata[0]]);
    __seqAck(bidib, bidibnode, MSG_BM_MIRROR_FREE, pdata, datasize);
    break;
  }

  case MSG_BM_MULTIPLE:
  {
    __handleMultipleSensors(bidib, bidibnode->uid, pdata, size);
    __seqAck(bidib, bidibnode, MSG_BM_MIRROR_MULTIPLE, pdata, datasize);
    break;
  }

  case MSG_FEATURE_COUNT:
  case MSG_FEATURE:
  case MSG_FEATURE_NA:
  {
    __handleNodeFeature(bidib, bidibnode, Type, size, pdata, datasize);
    break;
  }

  case MSG_BM_ADDRESS:
  { //             MNUM, ADDRL, ADDRH
    // 06 00 0C A3 04    5E     13 C4
    int locoAddr = (pdata[2]&0x3F) * 256 + pdata[1];
    int type = (pdata[2] >> 6);
    int port = pdata[0] & 0xFF;

    if( port > 127 ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,"BM port out of range: %d", port);
    }
    else {
      if( locoAddr > 0 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,"BM port %d reports loco %d", port, locoAddr);
        bidibnode->occ[port] = True;
        __handleSensor(bidib, bidibnode->uid, pdata[0], bidibnode->occ[port], locoAddr, type, bidibnode->bmload[port] );
      }
    }
    break;
  }

  case MSG_BM_CV:
  { //             ADDRL, ADDRH, CVL, CVH, DAT
    // 08 00 0D A5 5E     13     06   00   02 38
    int locoAddr = (pdata[1]&0x3F) * 256 + pdata[0];
    int cv       = pdata[3] * 256 + pdata[2];
    __handleCV(bidib, locoAddr, cv, pdata[4]);
    break;
  }


  case MSG_BM_SPEED:
  { //             ADDRL, ADDRH, DAT
    // 08 00 0D A6 5E     13     02
    // Unusable decoder speed information in KM/H; Just ignore it.
    int locoAddr = (pdata[1]&0x3F) * 256 + pdata[0];
    int speed    = pdata[2];
    iOSlot slot = __getSlotByAddr(bidib, locoAddr);
    if( slot != NULL && slot->kmh != speed ) {
      slot->kmh = speed;
      TraceOp.trc( name, TRCLEVEL_CALC, __LINE__, 9999,
          "loco=%s addr=%d speed=%dkm/h", slot->id, locoAddr, speed );

      iONode nodeC = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      if( data->iid != NULL )
        wLoc.setiid( nodeC, data->iid );
      wLoc.setid( nodeC, slot->id );
      wLoc.setaddr( nodeC, locoAddr );
      wLoc.setV_realkmh( nodeC, speed );
      wLoc.setcmd( nodeC, wLoc.bidikmh );
      data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
    }
    break;
  }


  case MSG_BM_CURRENT:
  { //             MNUM, DAT
    // 08 00 0D A7 00    00
    int port = pdata[0];
    int current = __calcCurrent(pdata[1]);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_BM_CURRENT, path=%s seq=%d port=%d current=%dmA", pathKey, Seq, port, current );
    if( bidibnode != NULL && port < 128) {
      bidibnode->bmload[port] = current;
      __handleSensor(bidib, bidibnode->uid, port, bidibnode->occ[port], 0, -1, bidibnode->bmload[port]);
    }
    break;
  }

  case MSG_SYS_ERROR:
  { // MSG_SYS_ERROR
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
        "MSG_SYS_ERROR, path=%s seq=%d error=%d", pathKey, Seq, pdata[0] );
    __handleError(bidib, pdata);
    break;
  }


  case MSG_NODE_NA:
  {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_NODE_NA, path=%s seq=%d na-node=%d", pathKey, Seq, pdata[0] );
    __SoD(bidib, NULL);
    break;
  }

  case MSG_SYS_P_VERSION:
  {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MSG_SYS_P_VERSION %d.%d", pdata[1], pdata[0] );
    break;
  }

  case MSG_PRG_CV_STAT:
  {
    __handlePT(bidib, pdata[0], pdata[1]);
    break;
  }

  case MSG_LC_STAT:
  {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_LC_STAT path=%s type=%d port=%d state=%d", pathKey, pdata[0], pdata[1], pdata[2] );
    __handleStat(bidib, bidibnode, pdata);
    break;
  }

  case MSG_SYS_IDENTIFY_STATE:
  {
    __handleIdentify(bidib, bidibnode, pathKey);
    break;
  }

  case MSG_LC_CONFIG:
  {
    __handleConfig(bidib, bidibnode, pdata);
    break;
  }

  case MSG_LC_NA: {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "MSG_LC_NA: msg=0x%02X, path=%s porttype=%d portnr=%d", Type, pathKey, pdata[0], pdata[1] );
    break;
  }

  case MSG_BOOST_STAT:
    __handleBoosterStat(bidib, bidibnode, pdata);
    break;

  case MSG_BOOST_CURRENT:
    __handleBoosterCurrent(bidib, bidibnode, pdata);
    break;

  case MSG_BOOST_DIAGNOSTIC:
    __handleBoosterDiagnostic(bidib, bidibnode, pdata, datasize);
    break;

  case MSG_CS_STATE:
    __handleCSStat(bidib, bidibnode, pdata);
    break;

  case MSG_CS_DRIVE_ACK:
    __handleCSDriveAck(bidib, bidibnode, pdata);
    break;

  case MSG_BM_CONFIDENCE:
    if( bidibnode != NULL ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
          "MSG_BM_CONFIDENCE: uid=%08X path=%s void=%d freeze=%d signal=%d", bidibnode->uid, pathKey, pdata[0], pdata[1], pdata[2] );
      if( bidibnode->conf_void != pdata[0] || bidibnode->conf_freeze != pdata[1] || bidibnode->conf_signal != pdata[2] ) {
        bidibnode->conf_void   = pdata[0];
        bidibnode->conf_freeze = pdata[1];
        bidibnode->conf_signal = pdata[2];

        data->power = (bidibnode->conf_signal == 0) ? True:False;
        __reportState(bidib, bidibnode, False, False);
      }
    }
    break;

  case MSG_FW_UPDATE_STAT:
    __handleUpdateStat(bidib, bidibnode, pdata);
    break;

  case MSG_LC_WAIT:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_LC_WAIT path=%s type=%d port=%d time=%d", pathKey, pdata[0], pdata[1], pdata[2] );
    break;

  case MSG_LC_MACRO_STATE:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_LC_MACRO_STATE path=%s macro=%d state=%d", pathKey, pdata[0], pdata[1] );
    if( pdata[1] == BIDIB_MACRO_OFF || pdata[1] == BIDIB_MACRO_START ) {
      /* modify pdata to match MSG_LC_STAT */
      pdata[2] = pdata[1]; /* state */
      pdata[1] = pdata[0]; /* address */
      pdata[0] = wProgram.porttype_macro; /* type */
      __handleStat(bidib, bidibnode, pdata);
    }
    else {
      /* macro modification confirmation:
       * BIDIB_MACRO_RESTORE
       * BIDIB_MACRO_SAVE
       * BIDIB_MACRO_DELETE
       * */
      __handleMacroStat(bidib, bidibnode->uid, pdata );
    }
    break;

  case MSG_LC_MACRO:
    /* macro get response */
    __handleMacroGet(bidib, bidibnode->uid, pdata );
    break;

  case MSG_LC_MACRO_PARA:
    /* macro get response */
    __handleMacroParaGet(bidib, bidibnode->uid, pdata );
    break;

  case MSG_ACCESSORY_PARA:
    /* accessory get response */
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_ACCESSORY_PARA path=%s port=%d param=%d datasize=%d", pathKey, pdata[0], pdata[1], size );
    __handleAccessoryParaGet(bidib, bidibnode->uid, pdata );
    break;

  case MSG_LC_KEY:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_LC_KEY path=%s port=%d state=%d", pathKey, pdata[0], pdata[1] );
    __handleSensor(bidib, bidibnode->uid, pdata[0], pdata[1] > 0 ? True:False, 0, -1, 0);
    break;

  case MSG_ACCESSORY_STATE:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_ACCESSORY_STATE path=%s port=%d aspect=%d", pathKey, pdata[0], pdata[1] );
    __handleAccessory(bidib, bidibnode->uid, pdata);
    break;

  case MSG_SYS_GET_MAGIC:
    if( data->magicOK ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "MSG_SYS_GET_MAGIC path=%s; already got the magic", pathKey );
    }
    break;

  case MSG_CS_ACCESSORY_ACK:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_CS_ACCESSORY_ACK path=%s addr=%d ack=%d", pathKey, pdata[0] + pdata[1]*256, pdata[2] );
    __handleAccessoryAck(bidib, bidibnode->uid, pdata);
    break;

  case MSG_CS_POM_ACK:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_CS_POM_ACK path=%s addr=%d ack=%d", pathKey, pdata[0] + pdata[1]*256, pdata[5] );
    break;

  case MSG_CS_DRIVE_MANUAL:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_CS_DRIVE_MANUAL path=%s addr=%d", pathKey, pdata[0] + pdata[1]*256 );
    __handleDriveManual(bidib, bidibnode->uid, pdata);
    break;

  case MSG_CS_DRIVE_EVENT:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_CS_DRIVE_EVENT path=%s addr=%d event=0x%02X %s",
        pathKey, pdata[0] + pdata[1]*256, pdata[2], (pdata[2]&0x01)?"***LOST***":"" );
    break;

  default:
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "UNSUPPORTED: msg=0x%02X, path=%s", Type, pathKey );
    TraceOp.dump ( name, TRCLEVEL_INFO, (char*)msg, size );
    break;

  }

  return data->magicOK;
}


static void __bidibReader( void* threadinst ) {
  iOThread    th    = (iOThread)threadinst;
  iOBiDiB     bidib = (iOBiDiB)ThreadOp.getParm( th );
  iOBiDiBData data  = Data(bidib);
  byte msg[256];
  byte path[4] = {0,0,0,0};
  int size = 0;
  int addr = 0;
  int value = 0;
  int port = 0;
  int magicreq = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "BiDiB reader started." );

  ThreadOp.sleep(50); /* resume some time to get it all being setup */

  data->commOK = data->subConnect((obj)bidib);

  while( data->run ) {

    if( !data->commOK) {
      data->magicOK = False;
      magicreq = 0;
      __resetSeq(bidib);
      ThreadOp.sleep(2500);
      data->commOK = data->subConnect((obj)bidib);
    }

    if(!data->commOK) {
      continue;
    }

    if( !data->magicOK && SystemOp.getTick() - data->lastMagicReq > 100 ) {
      /* no magic received; request again */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "magic request #%d", magicreq );
      magicreq++;
      data->lastMagicReq = SystemOp.getTick();
      path[0] = 0;
      data->subWrite((obj)bidib, path, MSG_SYS_GET_MAGIC, NULL, 0, 0);
    }

    if( !data->subAvailable( (obj)bidib) ) {
      ThreadOp.sleep( 10 );
      continue;
    }
    else {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "bidib message available" );
    }

    size = data->subRead( (obj)bidib, msg );
    if( size > 0 ) {
        __processBidiMsg(bidib, msg, size);
    }

  };

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "BiDiB reader ended." );
}


static int vmajor = 2;
static int vminor = 0;
static int patch  = 99;
static int _version( obj inst ) {
  iOBiDiBData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


/**  */
static struct OBiDiB* _inst( const iONode ini ,const iOTrace trc ) {
  iOBiDiB __BiDiB = allocMem( sizeof( struct OBiDiB ) );
  iOBiDiBData data = allocMem( sizeof( struct OBiDiBData ) );
  MemOp.basecpy( __BiDiB, &BiDiBOp, 0, sizeof( struct OBiDiB ), data );

  TraceOp.set( trc );
  SystemOp.inst();
  /* Initialize data->xxx members... */

  data->ini      = ini;
  data->bidibini = wDigInt.getbidib(data->ini);
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );

  data->run      = True;

  data->commOK  = False;
  data->magicOK = False;

  data->mux      = MutexOp.inst( NULL, True );
  data->nodemap  = MapOp.inst();
  data->localmap = MapOp.inst();
  data->swtime   = wDigInt.getswtime( ini );
  data->lcmap    = MapOp.inst();
  data->lcmux    = MutexOp.inst( NULL, True );

  data->nodepath[0] = 0;

  if( data->bidibini == NULL ) {
    data->bidibini = NodeOp.inst( wBiDiB.name(), data->ini, ELEMENT_NODE);
    NodeOp.addChild( data->ini, data->bidibini);
  }

  data->secAck    = wBiDiB.issecAck( data->bidibini );
  data->secAckInt = wBiDiB.getsecAckInt( data->bidibini );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "BiDiB %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "http://www.bidib.org/" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid     = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sublib  = %s", wDigInt.getsublib( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "secAck  = %s, interval=%dms",
      wBiDiB.issecAck( data->bidibini ) ? "enabled":"disabled", wBiDiB.getsecAckInt(data->bidibini) * 10 );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  /* choose interface: */
  if( StrOp.equals( wDigInt.sublib_default, wDigInt.getsublib( ini ) ) ||
      StrOp.equals( wDigInt.sublib_serial, wDigInt.getsublib( ini ) ) )
  {
    /* serial */
    data->subInit       = serialInit;
    data->subConnect    = serialConnect;
    data->subDisconnect = serialDisconnect;
    data->subRead       = serialRead;
    data->subWrite      = serialWrite;
    data->subAvailable  = serialAvailable;
  }
  else if( StrOp.equals( wDigInt.sublib_udp, wDigInt.getsublib( ini ) ) ) {
    /* serial */
    data->subInit       = udpInit;
    data->subConnect    = udpConnect;
    data->subDisconnect = udpDisconnect;
    data->subRead       = udpRead;
    data->subWrite      = udpWrite;
    data->subAvailable  = udpAvailable;
  }

  data->subInit((obj)__BiDiB);

  data->reader = ThreadOp.inst( "bidibreader", &__bidibReader, __BiDiB );
  ThreadOp.start( data->reader );

  instCnt++;
  return __BiDiB;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/bidib.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
