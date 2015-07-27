/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/


#include "rocrail/impl/operator_impl.h"

#include "rocrail/public/app.h"
#include "rocrail/public/car.h"
#include "rocrail/public/model.h"
#include "rocrail/public/loc.h"

#include "rocs/public/mem.h"
#include "rocs/public/strtok.h"

#include "rocrail/wrapper/public/Operator.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Car.h"
#include "rocrail/wrapper/public/FunCmd.h"

static int instCnt = 0;

static void __informLocos(iOOperator inst);


/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOOperatorData data = Data(inst);
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
  iOOperatorData data = Data((iOOperator)inst);
  return data->props;
}

static const char* __id( void* inst ) {
  iOOperatorData data = Data((iOOperator)inst);
  return wOperator.getid(data->props);
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/** ----- OOperator ----- */


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
static struct OOperator* _inst( iONode ini ) {
  iOOperator __Operator = allocMem( sizeof( struct OOperator ) );
  iOOperatorData data = allocMem( sizeof( struct OOperatorData ) );
  MemOp.basecpy( __Operator, &OperatorOp, 0, sizeof( struct OOperator ), data );

  /* Initialize data->xxx members... */
  data->props = ini;

  instCnt++;
  return __Operator;
}


static Boolean _cmd( iOOperator inst, iONode nodeA ) {
  iOOperatorData data = Data(inst);

  const char* nodename = NodeOp.getName( nodeA );
  const char* cmd      = wOperator.getcmd( nodeA );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "command %s:%s for operator %s",
      nodename, (cmd==NULL?"-":cmd), wOperator.getid(data->props) );

  if( StrOp.equals( wLoc.block, cmd ) ) {
    OperatorOp.setLocality(inst, wOperator.getlocation(nodeA));
  }

  else if( StrOp.equals( wOperator.emptycar, cmd ) ) {
    /* empty the carids */
    iOStrTok tok = StrTokOp.inst(wOperator.getcarids(nodeA), ',');
    while( StrTokOp.hasMoreTokens(tok) ) {
      const char* carid = StrTokOp.nextToken(tok);
      if( OperatorOp.hasCar(inst, carid) ) {
        iOCar car = ModelOp.getCar(AppOp.getModel(), carid);
        if( car != NULL ) {
          iONode cmd = NodeOp.inst( wCar.name(), NULL, ELEMENT_NODE );
          wCar.setcmd( cmd, wCar.status_empty );
          CarOp.cmd( car, cmd );
        }
      }
    }
    StrTokOp.base.del(tok);
    __informLocos(inst);
  }

  else if( StrOp.equals( wOperator.loadcar, cmd ) ) {
    /* load the carids */
    iOStrTok tok = StrTokOp.inst(wOperator.getcarids(nodeA), ',');
    while( StrTokOp.hasMoreTokens(tok) ) {
      const char* carid = StrTokOp.nextToken(tok);
      if( OperatorOp.hasCar(inst, carid) ) {
        iOCar car = ModelOp.getCar(AppOp.getModel(), carid);
        if( car != NULL ) {
          iONode cmd = NodeOp.inst( wCar.name(), NULL, ELEMENT_NODE );
          wCar.setcmd( cmd, wCar.status_loaded );
          CarOp.cmd( car, cmd );
        }
      }
    }
    StrTokOp.base.del(tok);
    __informLocos(inst);
  }

  else if( StrOp.equals( wOperator.addcar, cmd ) ) {
    /* add the carids */
    iOStrTok tok = StrTokOp.inst(wOperator.getcarids(nodeA), ',');
    while( StrTokOp.hasMoreTokens(tok) ) {
      const char* carid = StrTokOp.nextToken(tok);
      if( !OperatorOp.hasCar(inst, carid) ) {
        /* add the car */
        char* newCarIds = NULL;
        if( StrOp.len(wOperator.getcarids(data->props)) > 0 ) {
          newCarIds = StrOp.fmt("%s,%s", wOperator.getcarids(data->props), carid);
        }
        else {
          newCarIds = StrOp.fmt("%s", carid);
        }
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "add car [%s] to operator [%s]: [%s]", carid, wOperator.getid(data->props), newCarIds );
        wOperator.setcarids(data->props, newCarIds);
        StrOp.free(newCarIds);
      }
    }
    StrTokOp.base.del(tok);
    /* Broadcast to clients. */
    {
      iONode clone = (iONode)NodeOp.base.clone( data->props );
      AppOp.broadcastEvent( clone );
    }
    __informLocos(inst);
  }

  else if( StrOp.equals( wOperator.removecar, cmd ) ) {
    /* remove the carids */
    iOStrTok tok = StrTokOp.inst(wOperator.getcarids(nodeA), ',');
    while( StrTokOp.hasMoreTokens(tok) ) {
      const char* carid = StrTokOp.nextToken(tok);
      if( OperatorOp.hasCar(inst, carid) ) {
        char* newCarIds = NULL;
        iOStrTok cars = StrTokOp.inst(wOperator.getcarids(data->props), ',');
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "remove car [%s] to operator [%s]", carid, wOperator.getid(data->props) );
        while( StrTokOp.hasMoreTokens(cars) ) {
          const char* oldcarid = StrTokOp.nextToken(cars);
          if( !StrOp.equals(oldcarid, carid) ) {
            if( newCarIds != NULL && StrOp.len(newCarIds) > 0 ) {
              newCarIds = StrOp.cat(newCarIds, ",");
            }
            newCarIds = StrOp.cat(newCarIds, oldcarid);
          }
        }
        StrTokOp.base.del(cars);
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "operator [%s] cars=[%s]", wOperator.getid(data->props), newCarIds );
        wOperator.setcarids(data->props, newCarIds);
        StrOp.free(newCarIds);
      }
    }
    StrTokOp.base.del(tok);
    /* Broadcast to clients. */
    {
      iONode clone = (iONode)NodeOp.base.clone( data->props );
      AppOp.broadcastEvent( clone );
    }
    __informLocos(inst);
  }

  else {
    iOStrTok tok = StrTokOp.inst(wOperator.getcarids(data->props), ',');
    while( StrTokOp.hasMoreTokens(tok) ) {
      iOCar car = ModelOp.getCar(AppOp.getModel(), StrTokOp.nextToken(tok) );
      if( car != NULL ) {
        CarOp.cmd(car, (iONode)NodeOp.base.clone(nodeA));
      }
    }
    StrTokOp.base.del(tok);
  }

  nodeA->base.del(nodeA);
  return True;
}

static void _swapPlacing( iOOperator inst, iONode cmd ) {
  iOOperatorData data = Data(inst);
  iOStrTok tok = StrTokOp.inst(wOperator.getcarids(data->props), ',');

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "swap for operator %s", wOperator.getid(data->props) );

  while( StrTokOp.hasMoreTokens(tok) ) {
    iOCar car = ModelOp.getCar(AppOp.getModel(), StrTokOp.nextToken(tok) );
    if( car != NULL ) {
      CarOp.swapPlacing(car, cmd);
    }
  }
  StrTokOp.base.del(tok);
}


static void __informLocos(iOOperator inst) {
  iOOperatorData data = Data(inst);
  /* Inform locos */
  iOMap lcMap = ModelOp.getLocoMap(AppOp.getModel());
  iOLoc lc = (iOLoc)MapOp.first( lcMap );

  while( lc != NULL ) {
    iONode props = LocOp.base.properties(lc);
    if( StrOp.equals(wOperator.getid(data->props), wLoc.gettrain(props) )) {
      LocOp.getLen(lc);
      AppOp.broadcastEvent( (iONode)props->base.clone( props ) );
    }
    lc = (iOLoc)MapOp.next( lcMap );
  };
}

/**  */
static void _modify( struct OOperator* inst ,iONode props ) {
  iOOperatorData data = Data(inst);

  if( props != NULL ) {
    int cnt = NodeOp.getAttrCnt( props );
    int i = 0;
    for( i = 0; i < cnt; i++ ) {
      iOAttr attr = NodeOp.getAttr( props, i );
      const char* name  = AttrOp.getName( attr );
      const char* value = AttrOp.getVal( attr );
      NodeOp.setStr( data->props, name, value );
    }
    props->base.del(props);
  }

  /* Inform locos */
  __informLocos(inst);

  /* Broadcast to clients. */
  {
    iONode clone = (iONode)NodeOp.base.clone( data->props );
    AppOp.broadcastEvent( clone );
  }
}


static Boolean _hasCar( struct OOperator* inst, const char* id ) {
  iOOperatorData data = Data(inst);
  Boolean hascar = False;
  iOStrTok tok = StrTokOp.inst(wOperator.getcarids(data->props), ',');
  while( StrTokOp.hasMoreTokens(tok) ) {
    if( StrOp.equals(id, StrTokOp.nextToken(tok)) ) {
      hascar = True;
      break;
    }
  }
  StrTokOp.base.del(tok);
  return hascar;
}


static void __setFunctionCmd(iOOperatorData data, iONode cmd, Boolean flip, Boolean fon, int fnaction, int duration) {
  if( flip ) {
    wFunCmd.setf0 ( cmd, fnaction== 0?!wFunCmd.isf0 ( cmd ):wFunCmd.isf0 ( cmd ) );
    wFunCmd.setf1 ( cmd, fnaction== 1?!wFunCmd.isf1 ( cmd ):wFunCmd.isf1 ( cmd ) );
    wFunCmd.setf2 ( cmd, fnaction== 2?!wFunCmd.isf2 ( cmd ):wFunCmd.isf2 ( cmd ) );
    wFunCmd.setf3 ( cmd, fnaction== 3?!wFunCmd.isf3 ( cmd ):wFunCmd.isf3 ( cmd ) );
    wFunCmd.setf4 ( cmd, fnaction== 4?!wFunCmd.isf4 ( cmd ):wFunCmd.isf4 ( cmd ) );
    wFunCmd.setf5 ( cmd, fnaction== 5?!wFunCmd.isf5 ( cmd ):wFunCmd.isf5 ( cmd ) );
    wFunCmd.setf6 ( cmd, fnaction== 6?!wFunCmd.isf6 ( cmd ):wFunCmd.isf6 ( cmd ) );
    wFunCmd.setf7 ( cmd, fnaction== 7?!wFunCmd.isf7 ( cmd ):wFunCmd.isf7 ( cmd ) );
    wFunCmd.setf8 ( cmd, fnaction== 8?!wFunCmd.isf8 ( cmd ):wFunCmd.isf8 ( cmd ) );
    wFunCmd.setf9 ( cmd, fnaction== 9?!wFunCmd.isf9 ( cmd ):wFunCmd.isf9 ( cmd ) );
    wFunCmd.setf10( cmd, fnaction==10?!wFunCmd.isf10( cmd ):wFunCmd.isf10( cmd ) );
    wFunCmd.setf11( cmd, fnaction==11?!wFunCmd.isf11( cmd ):wFunCmd.isf11( cmd ) );
    wFunCmd.setf12( cmd, fnaction==12?!wFunCmd.isf12( cmd ):wFunCmd.isf12( cmd ) );
    wFunCmd.setf13( cmd, fnaction==13?!wFunCmd.isf13( cmd ):wFunCmd.isf13( cmd ) );
    wFunCmd.setf14( cmd, fnaction==14?!wFunCmd.isf14( cmd ):wFunCmd.isf14( cmd ) );
    wFunCmd.setf15( cmd, fnaction==15?!wFunCmd.isf15( cmd ):wFunCmd.isf15( cmd ) );
    wFunCmd.setf16( cmd, fnaction==16?!wFunCmd.isf16( cmd ):wFunCmd.isf16( cmd ) );
    wFunCmd.setf17( cmd, fnaction==17?!wFunCmd.isf17( cmd ):wFunCmd.isf17( cmd ) );
    wFunCmd.setf18( cmd, fnaction==18?!wFunCmd.isf18( cmd ):wFunCmd.isf18( cmd ) );
    wFunCmd.setf19( cmd, fnaction==19?!wFunCmd.isf19( cmd ):wFunCmd.isf19( cmd ) );
    wFunCmd.setf20( cmd, fnaction==20?!wFunCmd.isf20( cmd ):wFunCmd.isf20( cmd ) );
    wFunCmd.setf21( cmd, fnaction==21?!wFunCmd.isf21( cmd ):wFunCmd.isf21( cmd ) );
    wFunCmd.setf22( cmd, fnaction==22?!wFunCmd.isf22( cmd ):wFunCmd.isf22( cmd ) );
    wFunCmd.setf23( cmd, fnaction==23?!wFunCmd.isf23( cmd ):wFunCmd.isf23( cmd ) );
    wFunCmd.setf24( cmd, fnaction==24?!wFunCmd.isf24( cmd ):wFunCmd.isf24( cmd ) );
    wFunCmd.setf25( cmd, fnaction==25?!wFunCmd.isf25( cmd ):wFunCmd.isf25( cmd ) );
    wFunCmd.setf26( cmd, fnaction==26?!wFunCmd.isf26( cmd ):wFunCmd.isf26( cmd ) );
    wFunCmd.setf27( cmd, fnaction==27?!wFunCmd.isf27( cmd ):wFunCmd.isf27( cmd ) );
    wFunCmd.setf28( cmd, fnaction==28?!wFunCmd.isf28( cmd ):wFunCmd.isf28( cmd ) );
  }
  else {
    wFunCmd.setf0 ( cmd, fnaction== 0?fon:wFunCmd.isf0 ( cmd ) );
    wFunCmd.setf1 ( cmd, fnaction== 1?fon:wFunCmd.isf1 ( cmd ) );
    wFunCmd.setf2 ( cmd, fnaction== 2?fon:wFunCmd.isf2 ( cmd ) );
    wFunCmd.setf3 ( cmd, fnaction== 3?fon:wFunCmd.isf3 ( cmd ) );
    wFunCmd.setf4 ( cmd, fnaction== 4?fon:wFunCmd.isf4 ( cmd ) );
    wFunCmd.setf5 ( cmd, fnaction== 5?fon:wFunCmd.isf5 ( cmd ) );
    wFunCmd.setf6 ( cmd, fnaction== 6?fon:wFunCmd.isf6 ( cmd ) );
    wFunCmd.setf7 ( cmd, fnaction== 7?fon:wFunCmd.isf7 ( cmd ) );
    wFunCmd.setf8 ( cmd, fnaction== 8?fon:wFunCmd.isf8 ( cmd ) );
    wFunCmd.setf9 ( cmd, fnaction== 9?fon:wFunCmd.isf9 ( cmd ) );
    wFunCmd.setf10( cmd, fnaction==10?fon:wFunCmd.isf10( cmd ) );
    wFunCmd.setf11( cmd, fnaction==11?fon:wFunCmd.isf11( cmd ) );
    wFunCmd.setf12( cmd, fnaction==12?fon:wFunCmd.isf12( cmd ) );
    wFunCmd.setf13( cmd, fnaction==13?fon:wFunCmd.isf13( cmd ) );
    wFunCmd.setf14( cmd, fnaction==14?fon:wFunCmd.isf14( cmd ) );
    wFunCmd.setf15( cmd, fnaction==15?fon:wFunCmd.isf15( cmd ) );
    wFunCmd.setf16( cmd, fnaction==16?fon:wFunCmd.isf16( cmd ) );
    wFunCmd.setf17( cmd, fnaction==17?fon:wFunCmd.isf17( cmd ) );
    wFunCmd.setf18( cmd, fnaction==18?fon:wFunCmd.isf18( cmd ) );
    wFunCmd.setf19( cmd, fnaction==19?fon:wFunCmd.isf19( cmd ) );
    wFunCmd.setf20( cmd, fnaction==20?fon:wFunCmd.isf20( cmd ) );
    wFunCmd.setf21( cmd, fnaction==21?fon:wFunCmd.isf21( cmd ) );
    wFunCmd.setf22( cmd, fnaction==22?fon:wFunCmd.isf22( cmd ) );
    wFunCmd.setf23( cmd, fnaction==23?fon:wFunCmd.isf23( cmd ) );
    wFunCmd.setf24( cmd, fnaction==24?fon:wFunCmd.isf24( cmd ) );
    wFunCmd.setf25( cmd, fnaction==25?fon:wFunCmd.isf25( cmd ) );
    wFunCmd.setf26( cmd, fnaction==26?fon:wFunCmd.isf26( cmd ) );
    wFunCmd.setf27( cmd, fnaction==27?fon:wFunCmd.isf27( cmd ) );
    wFunCmd.setf28( cmd, fnaction==28?fon:wFunCmd.isf28( cmd ) );
  }

  wFunCmd.setfnchanged( cmd, fnaction );
  wFunCmd.settimedfn( cmd, fon?fnaction:-1 );
  wFunCmd.setgroup( cmd, fnaction/4 + ((fnaction%4 > 0) ? 1:0) );
  if( fon ) {
    wFunCmd.settimer( cmd, duration );
  }
}


static void __doCarFunction(iOOperatorData data, iOCar car, Boolean flip, Boolean fon, int fnaction, int duration) {
  iONode cmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "car function [%d] %s", fnaction, fon?"ON":"OFF" );
  CarOp.getFunctionStatus(car, cmd);
  __setFunctionCmd(data, cmd, flip, fon, fnaction, duration);
  CarOp.cmd( car, cmd);
}


static Boolean _setFnByDesc( iOOperator inst, const char* desc, Boolean flip, Boolean fon, int duration) {
  iOOperatorData data = Data(inst);
  Boolean foundFunction = False;

  iOStrTok tok = StrTokOp.inst(wOperator.getcarids(data->props), ',');
  while( StrTokOp.hasMoreTokens(tok) ) {
    iOCar car = ModelOp.getCar(AppOp.getModel(), StrTokOp.nextToken(tok) );
    if( car != NULL ) {
      int fnaction = CarOp.getFnNrByDesc(car, desc);
      if( fnaction != -1 ) {
        foundFunction = True;
        __doCarFunction(data, car, flip, fon, fnaction, duration);
      }
    }
  }
  StrTokOp.base.del(tok);

  return foundFunction;
}


static Boolean _isCommuter( struct OOperator* inst ) {
  iOOperatorData data = Data(inst);
  Boolean commuter = False;
  iOStrTok tok = StrTokOp.inst(wOperator.getcarids(data->props), ',');
  while( StrTokOp.hasMoreTokens(tok) ) {
    iOCar car = ModelOp.getCar(AppOp.getModel(), StrTokOp.nextToken(tok) );
    if( car != NULL && CarOp.isCommuter(car) ) {
      commuter = True;
      break;
    }
  }
  StrTokOp.base.del(tok);
  return commuter;
}


static const char* _getCargo( struct OOperator* inst ) {
  iOOperatorData data = Data(inst);
  return wOperator.getcargo(data->props);
}


static const char* _getClass( struct OOperator* inst ) {
  iOOperatorData data = Data(inst);
  return wOperator.getclass(data->props);
}


static Boolean _hasClass( iOOperator inst, const char* class ) {
  iOOperatorData data = Data(inst);
  const char* l_class = wOperator.getclass(data->props);
  return StrOp.find(l_class, class) != NULL ? True:False;
}


static void _setClass( struct OOperator* inst, const char* newclass ) {
  iOOperatorData data = Data(inst);
  iONode clone = NULL;
  wOperator.setclass(data->props, newclass);
  clone = (iONode)NodeOp.base.clone( data->props );
  AppOp.broadcastEvent( clone );
}


static int _getVMax( struct OOperator* inst ) {
  iOOperatorData data = Data(inst);
  int vmax = wOperator.getV_max(data->props);
  iOStrTok tok = StrTokOp.inst(wOperator.getcarids(data->props), ',');
  while( StrTokOp.hasMoreTokens(tok) ) {
    iOCar car = ModelOp.getCar(AppOp.getModel(), StrTokOp.nextToken(tok) );
    if( car != NULL ) {
      int vmaxcar = CarOp.getVMax(car);
      if( vmaxcar > 0 ) {
        if( vmax == 0 || vmaxcar < vmax)
          vmax = vmaxcar;
      }
    }
  }
  StrTokOp.base.del(tok);

  return vmax;
}


static void _setLocality( struct OOperator* inst, const char* bkid ) {
  iOOperatorData data = Data(inst);
  iOStrTok tok = StrTokOp.inst(wOperator.getcarids(data->props), ',');
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set locality to %s for operator %s", bkid==NULL?"-":bkid, wOperator.getid(data->props) );
  while( StrTokOp.hasMoreTokens(tok) ) {
    iOCar car = ModelOp.getCar(AppOp.getModel(), StrTokOp.nextToken(tok) );
    if( car != NULL ) {
      CarOp.setLocality(car, bkid);
    }
  }
  StrTokOp.base.del(tok);
  wOperator.setlocation(data->props, bkid);

  {
    iONode clone = (iONode)NodeOp.base.clone( data->props );
    AppOp.broadcastEvent( clone );
  }
}


static Boolean _matchIdent( struct OOperator* inst, const char* ident1, const char* ident2, const char* ident3, const char* ident4 ) {
  iOOperatorData data = Data(inst);
  Boolean match = False;
  iOStrTok tok = StrTokOp.inst(wOperator.getcarids(data->props), ',');
  while( StrTokOp.hasMoreTokens(tok) ) {
    iOCar car = ModelOp.getCar(AppOp.getModel(), StrTokOp.nextToken(tok) );
    if( car != NULL ) {
      if( ident1 != NULL && StrOp.len(ident1) > 0 && StrOp.equals(ident1, CarOp.getIdent(car) ) ) {
        match = True;
        break;
      }
      else if( ident2 != NULL && StrOp.len(ident2) > 0 && StrOp.equals(ident2, CarOp.getIdent(car) ) ) {
        match = True;
        break;
      }
      else if( ident3 != NULL && StrOp.len(ident3) > 0 && StrOp.equals(ident3, CarOp.getIdent(car) ) ) {
        match = True;
        break;
      }
      else if( ident4 != NULL && StrOp.len(ident4) > 0 && StrOp.equals(ident4, CarOp.getIdent(car) ) ) {
        match = True;
        break;
      }

    }
  }
  StrTokOp.base.del(tok);

  return match;
}


static int _getLen( struct OOperator* inst, int* trainweight ) {
  iOOperatorData data = Data(inst);
  /* ToDo: Calculate consist length. */
  int len = 0;
  int weight = 0;
  iOStrTok tok = StrTokOp.inst(wOperator.getcarids(data->props), ',');
  while( StrTokOp.hasMoreTokens(tok) ) {
    iOCar car = ModelOp.getCar(AppOp.getModel(), StrTokOp.nextToken(tok) );
    if( car != NULL ) {
      len += CarOp.getLen(car);
      weight += CarOp.getWeight(car);
    }
  }
  StrTokOp.base.del(tok);
  if( trainweight != NULL )
    *trainweight = weight;
  return len;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/operator.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
