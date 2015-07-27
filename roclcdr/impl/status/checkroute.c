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



void statusCheckRoute( iILcDriverInt inst ) {
  iOLcDriverData data = Data(inst);
  iONode lcprops = (iONode)data->loc->base.properties( data->loc );

  if( data->next1Route == NULL ) {
    ThreadOp.sleep(10);
    return;
  }

  if( !data->next1Route->isSet(data->next1Route) ) {
    /* not all switches are set; wait */
    ThreadOp.sleep(10);
    return;
  }
  else {
    /* Start engine and roll. */
    Boolean semaphore = False;
    Boolean dir = data->next1Route->getDirection( data->next1Route,
        data->loc->getCurBlock( data->loc ), &data->next1RouteFromTo );

    data->loc->depart(data->loc);

    semaphore = setSignals((iOLcDriver)inst, False);


    if( !data->gomanual ) {
      int departdelay=0;

      iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      /* Send the second command to the loc: */
      int maxkmh = 0;
      wLoc.setdir( cmd, dir );
      wLoc.setV_hint( cmd, getBlockV_hint(inst, data->curBlock, True, data->next1Route, !data->next1RouteFromTo, &maxkmh ) );
      wLoc.setV_maxkmh(cmd, maxkmh);

      if( !StrOp.equals( wLoc.getV_hint( cmd), wLoc.min ) && data->next1Route->hasThrownSwitch(data->next1Route) ) {
        if( data->loc->compareVhint( data->loc, wLoc.mid) == -1 || data->loc->getV( data->loc ) == 0 )
          wLoc.setV_hint( cmd, wLoc.mid );
      }

      if( data->loc->getV( data->loc ) == 0 ) {
        /* delay only if the loc is not running */
        if(semaphore) {
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201,
              "give the semaphore %dms time to get in position...", data->semaphoreWait );
          /* give the semaphore some time to get in position... */
          wLoc.setcmdDelay(cmd, data->semaphoreWait);
        }
        else if(data->signalWait > 0){
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201,
              "give the signal %dms time to set another aspect...", data->signalWait );
          wLoc.setcmdDelay(cmd, data->signalWait);
        }

        if( data->curBlock == NULL ) {
          /* loco was reset by user... */
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 4101, "curBlock is not set..." );
          return;
        }

        /* wait for departdelay if set for the current block*/
        departdelay = data->curBlock->getDepartDelay( data->curBlock ) ;
        if( wLoc.isusedepartdelay(lcprops) && departdelay > 0 ) {
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201, "delay departure for departdelay [%d] sec of block [%s].",
                         departdelay, data->loc->getCurBlock( data->loc) );
          wLoc.setcmdDelay(cmd, departdelay * 1000);
        }
      }

      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201,
                     "Setting direction for [%s] to [%s] at velocity [%s].",
                     data->loc->getId( data->loc ), dir?"forwards":"reverse",
                     wLoc.getV_hint(cmd) );

      /* Send the command to the loc: */
      data->loc->cmd( data->loc, cmd );
    }

    data->state = LC_PRE2GO;
    data->eventTimeout = 0;
    data->signalReset  = 0;
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201,
                   "Setting state for \"%s\" from LC_CHECKROUTE to LC_PRE2GO.",
                   data->loc->getId( data->loc ) );
  }
}

