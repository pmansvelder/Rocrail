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


void statusFindDest( iILcDriverInt inst ) {
  iOLcDriverData data = Data(inst);
  int scheduleIdx = data->scheduleIdx;
  Boolean mansignal = (data->curBlock->hasManualSignal(data->curBlock, False, False) != NULL ? True:False);

  /* Find a free destination. */
  if( data->schedule == NULL || StrOp.len( data->schedule ) == 0 ) {
    data->next1Block = data->model->findDest( data->model, data->loc->getCurBlock( data->loc ),
                                        NULL,
                                        data->loc, &data->next1Route, data->gotoBlock,
                                        False, data->opponly, False, False ); /* currently is no prev route running */
  }
  else {
    Boolean wait = False;

    if( scheduleIdx == 0 && !data->model->isScheduleFree(data->model, data->schedule, data->loc->getId(data->loc)) ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201,"schedule[%s] is not free2go", data->schedule);
      wait = True;
    }
    /* evaluate departure time */
    else if( wLoc.isusescheduletime( data->loc->base.properties( data->loc ) ) &&
        !checkScheduleTime( inst, data->schedule, data->prewaitScheduleIdx == -1 ? data->scheduleIdx:data->prewaitScheduleIdx ) ){
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201,
          "Waiting for schedule index[%d] (preWaitIdx=%d)",
          data->prewaitScheduleIdx == -1 ? data->scheduleIdx:data->prewaitScheduleIdx, data->prewaitScheduleIdx);
      data->warningnodestfound = True;
      wait = True;
    }

    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201, "checkScheduleEntryActions for entry %d...", scheduleIdx );
    checkScheduleEntryActions(inst, scheduleIdx, False);
    if( data->pendingSwap ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201, "pending swap for schedule" );
      data->loc->swapPlacing( data->loc, NULL, False, True );
      data->pendingSwap = False;
    }

    if( !wait ) {
      /* find destination using schedule */
      if( data->prewaitScheduleIdx != -1 ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201, "reset schedule index from %d to %d (preWait)", data->scheduleIdx, data->prewaitScheduleIdx );
        data->scheduleIdx = data->prewaitScheduleIdx;
        data->prewaitScheduleIdx = -1;
      }
      data->next1Route = data->model->calcRouteFromCurBlock( data->model, (iOList)NULL, data->schedule, &data->scheduleIdx,
                                                      data->loc->getCurBlock( data->loc ), NULL, data->loc, False, &data->indelay, False );
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201, "reported schedule index=%d", data->scheduleIdx );
    }

    if( !wait && data->next1Route != NULL ) {
      /* evaluate direction */
      if( StrOp.equals( data->next1Route->getToBlock( data->next1Route ), data->loc->getCurBlock( data->loc ) ) )
        data->next1Block = data->model->getBlock( data->model, data->next1Route->getFromBlock( data->next1Route ) );
      else
        data->next1Block = data->model->getBlock( data->model, data->next1Route->getToBlock( data->next1Route ) );
    }
    else if( wait ) {
      data->next1Block = NULL;
    }
    else {
      if( isScheduleEnd(inst) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 4201, "End of schedule: STOP." );
        checkScheduleEntryActions(inst, scheduleIdx, False);
        checkScheduleActions(inst, LC_FINDDEST);
        data->warningnodestfound = True;
      }
    }
  }


  data->curBlock  = data->model->getBlock( data->model, data->loc->getCurBlock( data->loc ) );

  if( data->next1Block != NULL && data->curBlock == data->next1Block ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 4101, "new destination block equals current block: stop" );
    data->schedule = NULL;
    data->next1Block = NULL;
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201, "reset next2Block" );
    resetNext2( (iOLcDriver)inst, True );
    data->next3Block = NULL;
    data->run = False;
  }

  if( data->next1Block != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 4201, "Found destination for [%s]: [%s] by route [%s]",
        data->loc->getId( data->loc ), data->next1Block->base.id( data->next1Block ), data->next1Route->base.id(data->next1Route) );

    data->loc->informBlock( data->loc, data->next1Block->base.id(data->next1Block), data->curBlock->base.id(data->curBlock) );

    data->state = LC_INITDEST;
    data->loc->setMode(data->loc, wLoc.mode_auto, wLoc.modereason_initdest);
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201, "Setting state for \"%s\" from LC_FINDDEST to LC_INITDEST.", data->loc->getId( data->loc ) );

  }
  else if(data->curBlock != NULL) {
    data->state = LC_WAITBLOCK;
    data->prevState = LC_FINDDEST;
    data->loc->setMode(data->loc, wLoc.mode_wait, wLoc.modereason_nodest);
    data->curBlock->didNotDepart(data->curBlock, data->loc->getId( data->loc ) );
    if( !data->warningnodestfound ) {
      data->warningnodestfound = True;
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 4101, "No destination found for [%s]; waiting...", data->loc->getId( data->loc ) );
    }
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201, "Setting state for [%s] from LC_FINDDEST to LC_WAITBLOCK.", data->loc->getId( data->loc ) );
  }
  else {
    if( !data->warningnodestfound ) {
      data->state = LC_IDLE;
      data->prevState = LC_FINDDEST;
      data->run = False;
      data->warningnodestfound = True;
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 4101,
          "No destination found for [%s]; current block [%s] not found...",
          data->loc->getId( data->loc ), data->loc->getCurBlock( data->loc ) == NULL ? "?":data->loc->getCurBlock( data->loc )  );
    }
  }

}

