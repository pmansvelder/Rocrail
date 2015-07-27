/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 All rights reserved.
*/

/*
    Code für DC-Car-Modus
    Eigenes PC-Programm für den DC-Car Modus:
    In die CV21 des DC-Car Decoders wird der Wert 24 programmiert.
    Einstellung der seriellen Schnittstelle 9600 Bps, 8 Bit, ODD Parity, 2 Stoppbit
    Es werden immer 3 Bytes als Block gesendet:
    1. Byte = Niedere Addresse mit der Bitfolge 00AAAAAA
    2. Byte = Hohe Addresse mit der Bitfolge 01AAAAAA
    3. Byte = Kommando Licht mit der Bitfolge 10CCCCCC oder Kommando Motor mit der Bitfolge 11MM1MMM

    Mit Byte 1 und 2 können bis zu 1023 Autos addressiert werden.

    Kommandos für Licht:
    10CCCCC1 Blinker links
    10CCCC1C Blinker recht
    10CCCC11 Warnblinker
    10CCC1CC Fahrlicht
    10CC1CCC Licht 2
    10C1CCCC Blaulicht
    101CCCCC Frontblitzer
    Kommandos für den Motor:
    11001000 Stop
    11001001 Fahrstufe 1
    11001010 Fahrstufe 2
    11001011 Fahrstufe 3
    11001100 Fahrstufe 4
    11001101 Fahrstufe 5
    11001110 Fahrstufe 6
    11001111 Fahrstufe 7
    11011000 Fahrstufe 8
    11011001 Fahrstufe 9
    11011010 Fahrstufe 10
    11011011 Fahrstufe 11
    11011100 Fahrstufe 12
    11011101 Fahrstufe 13
    11011110 Fahrstufe 14
    11011111 Fahrstufe 15
    11101000 Fahrstufe 16
    11101001 Fahrstufe 17
    11101010 Fahrstufe 18
    11101011 Fahrstufe 19
    11101100 Fahrstufe 20
    11101101 Fahrstufe 21
    11101110 Fahrstufe 22
    11101111 Fahrstufe 23
    11111000 Fahrstufe 24
    11111001 Fahrstufe 25
    11111010 Fahrstufe 26
    11111011 Fahrstufe 27
    11111100 Fahrstufe 28
    11111101 Fahrstufe 28
    11111110 Fahrstufe 28
    11111111 Fahrstufe 28
*/


#include "rocdigs/impl/dccar_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Command.h"
#include "rocrail/wrapper/public/FunCmd.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iODCCarData data = Data(inst);
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

/** ----- ODCCar ----- */

static void __translate( iODCCar inst, iONode node ) {
  iODCCarData data = Data(inst);


  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );
    if( StrOp.equals( cmdstr, wSysCmd.ebreak ) ) {
      /* CS ebreak */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request emergency break" );
      byte* cmd = allocMem(32);
      cmd[ 0] = 2;
      cmd[ 1] = 0xF0;
      cmd[ 2] = 0xF0;
      ThreadOp.post(data->writer, (obj)cmd);
    }
  }

  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int   addr = wLoc.getaddr( node );
    int  speed = 0;

    byte lsb = addr & 0x3F;
    byte msb = 0x40 + ((addr >> 6) & 0x0F);
    byte V   = 0xC8;
    /*
    1. Byte = Niedere Addresse mit der Bitfolge 00AAAAAA
    2. Byte = Hohe Addresse mit der Bitfolge 0100AAAA (Function group 1), 0110AAAA (Function group 2)
    3. Byte = Kommando Licht mit der Bitfolge 10CCCCCC oder Kommando Motor mit der Bitfolge 11MM1MMM
    */
    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * 32) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * 32) / wLoc.getV_max( node );
    }
    V |= speed & 0x07;
    V |= (speed << 1) & 0x30;

    byte* cmd = allocMem(32);
    cmd[ 0] = 3;
    cmd[ 1] = lsb;
    cmd[ 2] = msb;
    cmd[ 3] = V;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "dccar %d speed %d", addr, speed );
    ThreadOp.post(data->writer, (obj)cmd);
  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int   addr = wFunCmd.getaddr( node );

    /* Group 1 */
    byte lsb = addr & 0x3F;
    byte msb = 0x40 + ((addr >> 6) & 0x0F);
    byte fx  = 0x80;
    fx |= (wFunCmd.isf1(node)?0x01:0x00);
    fx |= (wFunCmd.isf2(node)?0x02:0x00);
    fx |= (wFunCmd.isf3(node)?0x04:0x00);
    fx |= (wFunCmd.isf4(node)?0x08:0x00);
    fx |= (wFunCmd.isf5(node)?0x10:0x00);
    fx |= (wFunCmd.isf6(node)?0x20:0x00);

    byte* cmd = allocMem(32);
    cmd[ 0] = 3;
    cmd[ 1] = lsb;
    cmd[ 2] = msb;
    cmd[ 3] = fx;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "dccar %d fx1 0x%02X", addr, fx );
    ThreadOp.post(data->writer, (obj)cmd);

    /* Group 2 */
    lsb = addr & 0x3F;
    msb = 0x60 + ((addr >> 6) & 0x0F);
    fx  = 0x80;
    fx |= (wFunCmd.isf0(node)?0x01:0x00);
    fx |= (wFunCmd.isf7(node)?0x02:0x00);
    fx |= (wFunCmd.isf8(node)?0x04:0x00);
    fx |= (wFunCmd.isf9(node)?0x08:0x00);
    fx |= (wFunCmd.isf10(node)?0x10:0x00);
    fx |= (wFunCmd.isf11(node)?0x20:0x00);

    cmd = allocMem(32);
    cmd[ 0] = 3;
    cmd[ 1] = lsb;
    cmd[ 2] = msb;
    cmd[ 3] = fx;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "dccar %d fx2 0x%02X", addr, fx );
    ThreadOp.post(data->writer, (obj)cmd);
}

}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iODCCarData data = Data(inst);

  if( cmd != NULL ) {
    int bus = 0;
    __translate( (iODCCar)inst, cmd );
    cmd->base.del(cmd);
  }
  return NULL;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff, Boolean shutdown ) {
  iODCCarData data = Data(inst);
  data->run = False;
  if( poweroff ) {
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
  ThreadOp.sleep(500);
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iODCCarData data = Data(inst);
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


static int vmajor = 2;
static int vminor = 0;
static int patch  = 99;
static int _version( obj inst ) {
  iODCCarData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iODCCar irc = (iODCCar)ThreadOp.getParm( th );
  iODCCarData data = Data(irc);
  byte* cmd = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "writer started." );
  ThreadOp.sleep(1000);

  while( data->run ) {
    byte * post = NULL;
    int len = 0;
    byte out[32] = {0};

    ThreadOp.sleep(10);
    post = (byte*)ThreadOp.getPost( th );

    if (post != NULL) {
      /* first byte is the message length */
      len = post[0];
      MemOp.copy( out, post+1, len);
      freeMem( post);

      TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, len );
      if( !SerialOp.write( data->serial, (char*)out, len ) ) {
        /* sleep and send it again? */
      }
      ThreadOp.sleep(data->writepause);
    }
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "writer ended." );
}


/**  */
static struct ODCCar* _inst( const iONode ini ,const iOTrace trc ) {
  iODCCar __DCCar = allocMem( sizeof( struct ODCCar ) );
  iODCCarData data = allocMem( sizeof( struct ODCCarData ) );
  MemOp.basecpy( __DCCar, &DCCarOp, 0, sizeof( struct ODCCar ), data );

  TraceOp.set( trc );
  SystemOp.inst();
  /* Initialize data->xxx members... */

  data->ini  = ini;
  data->iid  = StrOp.dup( wDigInt.getiid( ini ) );
  data->run  = True;
  data->writepause = wDigInt.getswtime( ini );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "DCCar %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid           = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device        = %s", wDigInt.getdevice( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "write pause   = %dms", data->writepause );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "line settings = 9600-8-2-O" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( wDigInt.getdevice( ini ) );
  SerialOp.setFlow( data->serial, StrOp.equals( wDigInt.cts, wDigInt.getflow( ini ) ) ? cts:0 );
  SerialOp.setLine( data->serial, 9600, 8, 2, odd, wDigInt.isrtsdisabled( ini ) );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), wDigInt.gettimeout( ini ) );

  data->serialOK = SerialOp.open( data->serial );

  if( data->serialOK ) {

    data->writer = ThreadOp.inst( "dccarwriter", &__writer, __DCCar );
    ThreadOp.start( data->writer );

  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init DCCcar port!" );
  }


  instCnt++;
  return __DCCar;
}

iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/dccar.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
