/*
 Rocs - OS independent C library

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rocs/impl/string_impl.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"


static int instCnt = 0;

/*
 ***** OBase operations.
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
  return Data(inst)->str;
}
static void __del(void* inst) {
  iOString     attr = inst;
  iOStringData data = Data(inst);
  StrOp.freeID( data->str, RocsStringID );
  freeIDMem( data, RocsStringID );
  freeIDMem( attr, RocsStringID );
  instCnt--;
}
static int __count(void) {
  return instCnt;
}
static Boolean __equals(void* inst1, void* inst2) {
  return StrOp.equals( StringOp.str(inst1), StringOp.str(inst2));
}
static obj __clone( void* inst ) {
  return (obj)StringOp.inst( StringOp.str( (iOString)inst ) );
}
static void* __properties(void* inst) {
  return NULL;
}

/*
 ***** OString operations.
 */
static const char* _str( iOString inst ) {
  iOStringData data = Data(inst);
  if( data == NULL )
    return "";
  return data->str != NULL ? data->str : "";
}

static int _len( iOString inst ) {
  iOStringData data = Data(inst);
  if( data == NULL )
    return 0;
  return data->len;
}

static iOString _inst( const char* val ) {
  iOString     obj  = allocIDMem( sizeof( struct OString     ), RocsStringID );
  iOStringData data = allocIDMem( sizeof( struct OStringData ), RocsStringID );

  /* OStringData */
  data->str = StrOp.dupID( val, RocsStringID );
  data->len = StrOp.len( data->str );

  /* OBase operations */
  MemOp.basecpy( obj, &StringOp, 0, sizeof( struct OString ), data );

  instCnt++;

  return obj;
}



/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/string.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
