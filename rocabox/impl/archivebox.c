/*
 Rocrail - Model Railroad Software
 Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net
 All rights reserved.
*/

#include "rocabox/impl/archivebox_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/file.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"

#include "rocabox/wrapper/public/ArchiveBox.h"
#include "rocabox/wrapper/public/Stub.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOArchiveBoxData data = Data(inst);
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

/** ----- OArchiveBox ----- */
static const char* _find( obj inst, const char* text ) {
  iOArchiveBoxData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "find [%s] in [%s]", text, data->home );
  return NULL;
}


/**  */
static struct OArchiveBox* _inst( const char* home ,const iOTrace trc ) {
  iOArchiveBox __ArchiveBox = allocMem( sizeof( struct OArchiveBox ) );
  iOArchiveBoxData data = allocMem( sizeof( struct OArchiveBoxData ) );
  MemOp.basecpy( __ArchiveBox, &ArchiveBoxOp, 0, sizeof( struct OArchiveBox ), data );

  TraceOp.set( trc );
  /* Initialize data->xxx members... */
  data->home = home;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "------------------------------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  ArchiveBox" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  home = [%s]", data->home );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  All rights reserved." );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "------------------------------------------------------------" );

  if( !FileOp.exist(data->home) ) {
    FileOp.mkdir(data->home);
  }

  instCnt++;
  return __ArchiveBox;
}

/* Support for dynamic Loading */
iOArchiveBox getArchiveBox( const char* home ,const iOTrace trc )
{
  return _inst(home,trc);
}



/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocabox/impl/archivebox.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
