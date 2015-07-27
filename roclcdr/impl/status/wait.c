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



void statusWait( iILcDriverInt inst, Boolean reverse ) {
  iOLcDriverData data = Data(inst);
  iONode bkprops = NULL;

  if( data->curBlock == NULL ) {
    iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    data->run = False;
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 4101, "no current block set for loco [%s]: stop auto mode.",  data->loc->getId( data->loc ) );
    wLoc.setV( cmd, 0 );
    data->loc->cmd( data->loc, cmd );

    data->state = LC_IDLE;
    data->loc->setMode(data->loc, wLoc.mode_idle, "");
    return;
  }

  bkprops = (iONode)data->curBlock->base.properties( data->curBlock );
  /* Station wait or all destinations are occupied. */
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201, "Wait in block for [%s]...",
                 data->loc->getId( data->loc ) );


  {

    data->state = LC_TIMER;
    data->loc->setMode(data->loc, wLoc.mode_wait, "");
    Boolean oppwait = True;
    int    ioppwait = 0;
    Boolean wait    = data->curBlock->wait(data->curBlock, data->loc, reverse, &oppwait );
    Boolean mainline =  wBlock.ismainline(data->curBlock->base.properties(data->curBlock) );


    if( wait ) {
      Boolean ice = StrOp.equals( wLoc.cargo_ice, wLoc.getcargo( data->loc->base.properties( data->loc ) ) );
      if( (mainline || ice) && data->prevState == LC_FINDDEST )
        data->timer = 1 * wLoc.getpriority( data->loc->base.properties( data->loc ) ); /* just wait 100ms multiplied by prio */
      else {
        data->timer = data->curBlock->getWait( data->curBlock, data->loc, reverse, &ioppwait );

        if( data->timer != -1 ) {
          if( data->prevState == LC_FINDDEST )
            data->timer = data->timer * wLoc.getpriority( data->loc->base.properties( data->loc ) );
          else
            data->timer = data->timer * 10;
        }
      }
    }
    else {
      Boolean ice = StrOp.equals( wLoc.cargo_ice, wLoc.getcargo( data->loc->base.properties( data->loc ) ) );
      if( (mainline || ice) && data->prevState == LC_FINDDEST )
        data->timer = 1 * wLoc.getpriority( data->loc->base.properties( data->loc ) ); /* just wait 100ms multiplied by prio */
      else
        data->timer = 5 * wLoc.getpriority( data->loc->base.properties( data->loc ) ); /* just wait 1 second, 10 x 100ms */
    }


    data->curBlock->resetTrigs( data->curBlock );
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201,
                   "Setting state for [%s] timer=%d from LC_WAITBLOCK to LC_TIMER.",
                   data->loc->getId( data->loc ), data->timer );
  }
}
