/*
 Rocs - OS independent C library

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#if defined __linux__ || defined _AIX || defined __unix__ || defined __APPLE__

#include "rocs/impl/lib_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/str.h"

#include <stdlib.h>
#include <string.h>

#ifndef _REENTRANT
#define _REENTRANT
#endif

#ifndef _REENTRANT
#define __USE_POSIX
#endif

#include <dlfcn.h>

#ifndef __ROCS_LIB__
	#pragma message("*** Unix OLib is disabled. (define __ROCS_LIB__ in rocs.h) ***")
#endif
/*
 ***** __Private functions.
 */
Boolean rocs_lib_load( iOLibData o ) {
#ifdef __ROCS_LIB__
  char* error = NULL;

#if defined __APPLE__
  if( !StrOp.endsWith( o->name, ".dylib" ) )
    o->name = StrOp.catID( o->name, ".dylib", RocsLibID );
#else
  if( !StrOp.endsWith( o->name, ".so" ) )
    o->name = StrOp.catID( o->name, ".so", RocsLibID );
#endif

  dlerror();
  o->lh = dlopen( o->name, RTLD_LAZY);
  error = dlerror();

  if( o->lh == NULL ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
                   "rocs_lib_load [%s] failed. rc=%s", o->name != NULL ? o->name:"", error );
    return False;
  }

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_lib_load OK [%s]", o->name != NULL ? o->name:"" );
#endif
  return True;
}

void* rocs_lib_getProc( iOLib inst, const char* procname ) {
  void* proc = NULL;
#ifdef __ROCS_LIB__
  iOLibData o = Data(inst);
  char* error = NULL;

  dlerror();
  proc = dlsym( o->lh, procname );
  error = dlerror();


  if( proc == NULL ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_lib_getProc [%s.%s] failed. rc=%s", o->name, procname, error );
    return NULL;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_lib_getProc OK [%s.%s]", o->name, procname );
#endif
  return proc;
}

#endif
