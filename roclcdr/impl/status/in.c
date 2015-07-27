/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#include "roclcdr/impl/lcdriver_impl.h"

#include "roclcdr/impl/tools/tools.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"



#include "rocrail/public/model.h"

#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/Schedule.h"
#include "rocrail/wrapper/public/ScheduleEntry.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Link.h"



void statusIn( iILcDriverInt inst ) {
  iOLcDriverData data = Data(inst);

  /* Signal of destination block. (_event) */
  if( data->next2Block == NULL ) {
    /* lp:786421 Also the gomanual trains must halt if no destination is available. */
    if( data->next1Block->hasExtStop(data->next1Block, data->loc->getId(data->loc)) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 4201,
          "block %s has a stop module; not sending velocity 0 to loco %s",
          data->next1Block->base.id(data->next1Block), data->loc->getId(data->loc));
    }
    else if( !data->gomanual || ( data->gomanual && data->stopatin4gomanual ) ) {
      iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setV( cmd, 0 );
      wLoc.setdir( cmd, wLoc.isdir( data->loc->base.properties( data->loc ) ) );
      data->loc->cmd( data->loc, cmd );
    }

    data->state = LC_WAITBLOCK;
    data->prevState = LC_INBLOCK;

    data->loc->setMode(data->loc, wLoc.mode_wait, "");
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201,
                   "Setting state for \"%s\" from LC_INBLOCK to LC_WAITBLOCK.",
                   data->loc->getId( data->loc ) );

  }
  else if( data->next1Route != NULL && !data->next1Route->isSet(data->next1Route) ) {
    /* stop and go in status checkroute */

    if( !data->gomanual ) {
      iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setV( cmd, 0 );
      wLoc.setdir( cmd, wLoc.isdir( data->loc->base.properties( data->loc ) ) );
      data->loc->cmd( data->loc, cmd );
    }

    data->next1Block = data->next2Block;
    data->next2Block = data->next3Block;
    data->next3Block = NULL;
    data->next1Block->link( data->next1Block, data->curBlock );
    data->next1Route->link(data->next1Route, data->curBlock->getTDport(data->curBlock));
    data->next1RouteFromTo = data->next2RouteFromTo;
    data->next2RouteFromTo = data->next3RouteFromTo;

    data->state = LC_CHECKROUTE;

    data->loc->setMode(data->loc, wLoc.mode_auto, wLoc.modereason_checkroute);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 4201,
                   "Waiting for route, setting state for [%s] from LC_INBLOCK to LC_CHECKROUTE.",
                   data->loc->getId( data->loc ) );


  }
  else {
    /* set the block departure velocity: */
    if( !data->gomanual && !data->didReduceSpeedAtEnter) {
      iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      int maxkmh = 0;
      wLoc.setV_hint( cmd, getBlockV_hint(inst, data->next1Block, True, data->next1Route, !data->next1RouteFromTo, &maxkmh ) );
      wLoc.setV_maxkmh(cmd, maxkmh);

      /* check for thrown switches in route */
      if( !StrOp.equals( wLoc.getV_hint( cmd), wLoc.min ) && data->next1Route->hasThrownSwitch(data->next1Route) ) {
        if( data->loc->compareVhint( data->loc, wLoc.mid) == -1 || data->loc->getV( data->loc ) == 0 )
          wLoc.setV_hint( cmd, wLoc.mid );
      }

      wLoc.setdir( cmd, wLoc.isdir( data->loc->base.properties( data->loc ) ) );
      data->loc->cmd( data->loc, cmd );
    }
    data->didReduceSpeedAtEnter = False;

    data->next1Block = data->next2Block;
    data->next2Block = data->next3Block;
    data->next3Block = NULL;
    data->next1Block->link( data->next1Block, data->curBlock );
    data->next1Route->link(data->next1Route, data->curBlock->getTDport(data->curBlock));
    data->next1RouteFromTo = data->next2RouteFromTo;
    data->next2RouteFromTo = data->next3RouteFromTo;

    data->state = LC_PRE2GO;

    data->loc->setMode(data->loc, wLoc.mode_auto, "");
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201,
                   "Setting state for \"%s\" from LC_INBLOCK to LC_PRE2GO.",
                   data->loc->getId( data->loc ) );
  }
}
