/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/


#include "rocrail/impl/dec_impl.h"

#include "rocs/public/mem.h"

#include "rocrail/public/app.h"

#include "rocrail/wrapper/public/CVByte.h"
#include "rocrail/wrapper/public/Dec.h"
#include "rocrail/wrapper/public/ModelCmd.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    /*
    iODecData data = Data(inst);
    freeMem( data );
    freeMem( inst );
    instCnt--;
    */
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

/** ----- ODec ----- */


/**  */
static char* _getForm( void* object ) {
  return 0;
}


/**  */
static char* _postForm( void* object ,const char* data ) {
  return 0;
}


/**  */
static const char* _tableHdr( void ) {
  return 0;
}


/**  */
static char* _toHtml( void* object ) {
  return 0;
}


/**  */
static int _getCV( iONode props ,int nr ) {
  iONode cv = NodeOp.findNode( props, wCVByte.name() );
  while( cv != NULL ) {
    if(wCVByte.getnr( cv ) == nr ) {
      return wCVByte.getvalue( cv );
    }
    cv = NodeOp.findNextNode( props, cv );
  };
  return 0;
}


/**  */
static void _setCV( iONode props ,int nr ,int value ) {
  Boolean existingcv = False;
  iONode cv = NodeOp.findNode( props, wCVByte.name() );
  while( cv != NULL ) {
    if(wCVByte.getnr( cv ) == nr ) {
      wCVByte.setvalue( cv, value );
      existingcv = True;
      break;
    }
    cv = NodeOp.findNextNode( props, cv );
  };
  if( !existingcv ) {
    cv = NodeOp.inst( wCVByte.name(), props, ELEMENT_NODE );
    wCVByte.setnr( cv, nr );
    wCVByte.setvalue( cv, value );
    NodeOp.addChild( props, cv );
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s: set cv %d to %d", wDec.getid(props), nr, value);
  /* Broadcast to clients. */
  {
    iONode clone = (iONode)NodeOp.base.clone( props );
    AppOp.broadcastEvent( clone );
  }

}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/dec.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
