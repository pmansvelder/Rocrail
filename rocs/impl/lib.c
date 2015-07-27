/*
 Rocs - OS independent C library

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#include <stdlib.h>
#include <string.h>

#include "rocs/impl/lib_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"

static int instCnt = 0;

/*
 ***** __Private functions.
 */
/* OS dependent */
Boolean rocs_lib_load( iOLibData o );
const void* rocs_lib_getProc( iOLib inst, const char* );


/*
 ***** OBase functions.
 */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static const char* __name(void) {
  return name;
}
static unsigned char* __serialize(void* inst, long* size) {
  return NULL;
}
static void __deserialize(void* inst, unsigned char* a) {
}
static char* __toString(void* inst) {
  iOLibData data = Data(inst);
  return StrOp.catID( "OLib: ", data->name != NULL ? data->name:"<unnamed>", RocsLibID );
}
static void __del(void* inst) {
  iOLibData data = Data(inst);
  StrOp.freeID( data->name, RocsLibID );
  freeIDMem( data, RocsLibID );
  freeIDMem( inst, RocsLibID );
  instCnt--;
}
static int __count(void) {
  return instCnt;
}
static void* __properties(void* inst) {
  return NULL;
}
static struct OBase* __clone( void* inst ) {
  return NULL;
}
static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}


/*
 ***** OLib public functions.
 */
static iOLib _inst( const char* name ) {
  iOLib     lib  = allocIDMem( sizeof( struct OLib ), RocsLibID );
  iOLibData data = allocIDMem( sizeof( struct OLibData ), RocsLibID );

  MemOp.basecpy( lib, &LibOp, 0, sizeof( struct OLib ), data );

  data->name = StrOp.dupID( name, RocsLibID );

  rocs_lib_load(data);

  instCnt++;

  return lib;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/lib.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
