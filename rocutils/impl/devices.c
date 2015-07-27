/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/


#include "rocutils/impl/devices_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/system.h"
#include "rocs/public/dir.h"
#include "rocs/public/str.h"
#include "rocs/public/serial.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
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

/** ----- ODevices ----- */


/**  */
static void _freeList( iOList list ) {
  int cnt = ListOp.size(list);
  int i = 0;
  for( i = 0; i < cnt; i++ ) {
    StrOp.free((char*)ListOp.get(list, i));
  }
  ListOp.base.del(list);
}


static char* _getDevicesStr( void ) {
  char* devlist = StrOp.dup("");
  iOList list = DevicesOp.getDevices();
  int cnt = ListOp.size(list);
  int i = 0;
  for( i = 0; i < cnt; i++ ) {
    if( i > 0 )
      devlist = StrOp.cat(devlist, ",");
    devlist = StrOp.cat(devlist, (const char*)ListOp.get(list, i));
  }

  DevicesOp.freeList(list);

  return devlist;
}

static Boolean __isDeviceAvailable(const char* devname) {
  Boolean available = False;
  iOSerial serial = SerialOp.inst( devname );
  if( SerialOp.open( serial ) ) {
    available = True;
  }
  SerialOp.base.del(serial);

  return available;
}

static iOList _getDevices( void ) {
  iODir dir = NULL;
  iOList list = ListOp.inst();
  const char* fileName = NULL;
  const char* path = NULL;
  const char* wildcard = NULL;

#if defined __APPLE__
  path = "/dev";
  wildcard = "tty.";
#elif defined __linux__
  path = "/dev";
  wildcard = "ttyUSB";
  {
    int i = 0;
    for( i = 0; i < 2; i++ ) {
      char* ttyS = StrOp.fmt( "/dev/ttyS%d", i );
      if( __isDeviceAvailable(ttyS) )
        ListOp.add(list, (obj)ttyS);
      else
        StrOp.free(ttyS);
      ttyS = StrOp.fmt( "/dev/ttyACM%d", i );
      if( __isDeviceAvailable(ttyS) )
        ListOp.add(list, (obj)ttyS);
      else
        StrOp.free(ttyS);
    }
  }
#elif defined _WIN32
  {
    int i = 0;
    for( i = 1; i < 10; i++ ) {
      char* com = StrOp.fmt( "COM%d", i );
      if( __isDeviceAvailable(com) )
        ListOp.add(list, (obj)com);
      else
        StrOp.free(com);
    }
  }
#endif

  if( path != NULL && wildcard != NULL ) {
    dir = DirOp.inst( path );
    if( dir != NULL ) {
      /* Get the first directory entry. */
      fileName = DirOp.read( dir );
      /* Iterate all directory entries. */
      while( fileName != NULL ) {
        if( StrOp.find( fileName, wildcard ) ) {
          char* devicepath = StrOp.fmt( "%s%c%s", path, SystemOp.getFileSeparator(), fileName );
          ListOp.add(list, (obj)devicepath);
        }
        /* Get the next directory entry. */
        fileName = DirOp.read( dir );
      }
    }
    DirOp.base.del(dir);
  }

  return list;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocutils/impl/devices.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
