/*
 Rocrail - Model Railroad Software
 Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net
 All rights reserved.
*/

#include "rocview/public/abox.h"

#include "rocs/public/lib.h"
#include "rocs/public/trace.h"

typedef iOArchiveBox (* LPFNGETARCHIVEBOX)( const iONode, const iOTrace );

ABox::ABox() {
  m_ArchiveBox = NULL;

  iOLib pLib = LibOp.inst("./rocabox");
  if( pLib != NULL ) {
    LPFNGETARCHIVEBOX  pInitFun = (LPFNGETARCHIVEBOX)LibOp.getProc( pLib, "getArchiveBox" );
    if (pInitFun != NULL) {
      m_ArchiveBox = pInitFun( NULL, TraceOp.get() );
      m_ArchiveBox->find("Zoek iets");
    }
  }
}
