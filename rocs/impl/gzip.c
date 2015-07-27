/*
 Rocs - OS independent C library

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rocs/impl/gzip_impl.h"
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
static void* __properties(void* inst) {
  return NULL;
}
static struct OBase* __clone( void* inst ) {
  return NULL;
}
static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}

static char* __toString(void* inst) {
  return NULL;
}

static void __del(void* inst) {
  iOGZip     attr = inst;
  iOGZipData data = Data(inst);
  freeMem( data );
  freeMem( attr );
  instCnt--;
}
static int __count(void) {
  return instCnt;
}

/*
 ***** OGZip operations.
 */
static int _getRc( iOGZip inst ) {
  if( inst == NULL )
    return 0;
  return Data(inst)->rc;
}


static Boolean _compress( iOGZip inst ) {
  iOGZipData data = Data(inst);
  if( data == NULL )
    return False;
  return False;
}

static Boolean _deCompress( iOGZip inst ) {
  iOGZipData data = Data(inst);
  if( data == NULL )
    return False;
  return False;
}

static iOGZip _inst( const char* name ) {
  iOGZip     obj  = allocMem( sizeof( struct OGZip     ) );
  iOGZipData data = allocMem( sizeof( struct OGZipData ) );

  /* OGZipData */
  data->name = StrOp.dup( name );

  /* OBase operations */
  MemOp.basecpy( obj, &GZipOp, 0, sizeof( struct OGZip ), data );

  instCnt++;

  return obj;
}



/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/gzip.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/

