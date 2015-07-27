/*
 Rocs - OS independent C library

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifdef _WIN32

#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "rocs/impl/lib_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/str.h"

#ifndef __ROCS_LIB__
	#pragma message("*** Win32 OLib is disabled. (define __ROCS_LIB__ in rocs.h) ***")
#endif
/*
 ***** __Private functions.
 */
Boolean rocs_lib_load( iOLibData o ) {
#ifdef __ROCS_LIB__
  if( !StrOp.endsWith( o->name, ".dll" ) )
    o->name = StrOp.catID( o->name, ".dll", RocsLibID );

  o->lh = LoadLibrary( o->name );
  if( (unsigned int)o->lh <= 32 ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_lib_load [%s] failed. rc=%d", o->name != NULL ? o->name:"", GetLastError() );
    return False;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocs_lib_load OK [%s]", o->name != NULL ? o->name:"" );
#endif
  return True;
}

const void* rocs_lib_getProc( iOLib inst, const char* procname ) {
  void* proc = NULL;
#ifdef __ROCS_LIB__
  iOLibData o = Data(inst);
  proc = GetProcAddress( o->lh, procname );
  if( proc == NULL || IsBadCodePtr(proc) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_lib_getProc [%s:%s] failed. rc=%d", o->name, procname, GetLastError() );
    return NULL;
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocs_lib_getProc OK [%s:%s]", o->name, procname );
#endif
  return proc;
}

#endif
