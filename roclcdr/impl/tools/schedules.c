/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#include <time.h>

#include "roclcdr/impl/lcdriver_impl.h"

#include "roclcdr/impl/tools/tools.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"



#include "rocrail/public/model.h"
#include "rocrail/public/action.h"

#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/Schedule.h"
#include "rocrail/wrapper/public/ScheduleEntry.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Link.h"
#include "rocrail/wrapper/public/ActionCtrl.h"



static Boolean isHourlyInRange(iILcDriverInt inst, iONode schedule) {
  iOLcDriverData data = Data(inst);
  Boolean inRange  = True;
  int     fromhour = wSchedule.getfromhour(schedule);
  int     tohour   = wSchedule.gettohour(schedule);
  int     hours    = 0;
  int     mins     = 0;

  long modeltime = data->model->getTime( data->model );
  struct tm* ltm;

  ltm = localtime( &modeltime );
  hours = ltm->tm_hour;
  mins  = ltm->tm_min;

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4203, "modeltime %02d:%02d (%ld)",
      hours, mins, modeltime );

  if( hours < fromhour || tohour < hours || tohour == hours ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4203,
        "current hour, %d, is no longer in the hourly range from %d to %d",
        hours, fromhour, tohour );
    inRange = False;
  }


  return inRange;
}



void checkScheduleActions( iILcDriverInt inst, int state) {
  iOLcDriverData data = Data(inst);

  /* reset schedule index */
  data->scheduleIdx = 0;
  data->prewaitScheduleIdx = -1;
  data->scheduleCycle++;

  if( data->schedule != NULL ) {
    iONode sc = data->model->getSchedule( data->model, data->schedule );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 4203, "check schedule actions [%s]", data->schedule );

    if( sc != NULL ) {
      const char* scaction = wSchedule.getscaction(sc);
      int           cycles = wSchedule.getcycles(sc);
      iONode    actionctrl = wSchedule.getactionctrl(sc);

      while( actionctrl != NULL ) {
        iOAction action = data->model->getAction(data->model, wActionCtrl.getid(actionctrl) );
        if( action != NULL ) {
          wActionCtrl.setlcid( actionctrl, data->loc->getId( data->loc ) );
          action->exec(action, actionctrl );
        }
        actionctrl = wSchedule.nextactionctrl(sc, actionctrl);
      };

      if( wSchedule.gettimeprocessing(sc) == wSchedule.time_hourly && isHourlyInRange(inst, sc) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 4203, "hourly schedule is recycled", scaction );
        /* set the schedule start time: */
        data->scheduletime = data->model->getTime( data->model );
        data->scheduleCycle = 0;

        if( state == LC_FINDDEST )
          data->next1Block = NULL;

        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4203, "reset next2Block" );
        resetNext2( (iOLcDriver)inst, True );
      }
      else if( cycles > 0 && data->scheduleCycle < cycles ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 4203, "next schedule cycle[%d] of %d activated", data->scheduleCycle, cycles );
        if( state == LC_FINDDEST )
          data->next1Block = NULL;

        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4203, "reset next2Block" );
        resetNext2( (iOLcDriver)inst, True );
      }
      /* check for a next action */
      else if( scaction != NULL && StrOp.len(scaction) > 0  ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 4203, "next schedule [%s] activated", scaction );
        /* set the schedule start time: */
        data->scheduletime = data->model->getTime( data->model );
        data->scheduleCycle = 0;
        data->schedule = scaction;
        if( state == LC_FINDDEST )
          data->next1Block = NULL;

        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4203, "reset next2Block" );
        resetNext2( (iOLcDriver)inst, True );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 4203, "no new schedule" );
        data->schedule = NULL;
        data->scheduleCycle = 0;
        if( state == LC_FINDDEST )
          data->next1Block = NULL;
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4203, "reset next2Block" );
        resetNext2( (iOLcDriver)inst, True );
        if( data->tour == NULL ) {
          data->run = False;
        }
      }

    }
    else {
      data->schedule = NULL;
      data->scheduleCycle = 0;
      if( state == LC_FINDDEST )
        data->next1Block = NULL;
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4203, "reset next2Block" );
      resetNext2( (iOLcDriver)inst, True );
      data->run = False;
    }
  }
}

Boolean checkScheduleEntryActions( iILcDriverInt inst, int index, Boolean checkSwapOnly ) {
  iOLcDriverData data = Data(inst);
  int scheduleIdx = (index == -1 ? data->scheduleIdx:index);

  if( data->schedule != NULL ) {
    iONode sc = data->model->getSchedule( data->model, data->schedule );
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4203,
        "check schedule entry actions [%s:%d] %s",
        data->schedule, scheduleIdx, checkSwapOnly?"Swap only":"" );

    if( sc != NULL && scheduleIdx < NodeOp.getChildCnt(sc)) {
      iONode entry = NodeOp.getChild(sc,scheduleIdx);
      if( entry != NULL ) {
        if( !checkSwapOnly && data->entryActionsChecked != index ) {
          iONode actionctrl = wScheduleEntry.getactionctrl(entry);
          data->entryActionsChecked = index;

          while( actionctrl != NULL ) {
            iOAction action = data->model->getAction(data->model, wActionCtrl.getid(actionctrl) );
            if( action != NULL ) {
              wActionCtrl.setlcid( actionctrl, data->loc->getId( data->loc ) );
              action->exec(action, actionctrl);
            }
            actionctrl = wSchedule.nextactionctrl(entry, actionctrl);
          };
        }

        return wScheduleEntry.isswap(entry);
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 4203,
          "schedule index %d is out of bounds for schedule %s",
          scheduleIdx, data->schedule );
    }
  }
  return False;
}


/**
 * check the departure time
 * return True if the train should go
 */
Boolean checkScheduleTime( iILcDriverInt inst, const char* scheduleID, int scheduleIdx ) {
  iOLcDriverData data = Data(inst);
  Boolean go = False;
  iONode schedule = data->model->getSchedule( data->model, scheduleID );
  if( data->state == LC_FINDDEST && data->curBlock != NULL ) {
    int i = scheduleIdx;
    scheduleIdx = data->model->getScheduleIndex( data->model, scheduleID, data->curBlock->base.id(data->curBlock), data->loc );
    if( i != scheduleIdx )
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "corrected scheduleIdx from %d to %d", i, scheduleIdx );
  }

  if( schedule != NULL ) {
    int idx = 0;
    int timeprocessing = wSchedule.gettimeprocessing(schedule);
    int timeframe      = wSchedule.gettimeframe(schedule);
    int fromhour       = wSchedule.getfromhour(schedule);
    int tohour         = wSchedule.gettohour(schedule);
    int maxdelay       = wSchedule.getmaxdelay(schedule);

    iONode entry = wSchedule.getscentry( schedule );

    /* check if the schedule index is correct: */
    while( entry != NULL ) {
      if( idx == scheduleIdx ) {
        long modeltime = data->model->getTime( data->model );
        struct tm* ltm;
        int modelminutes    = 0;
        int scheduleminutes = 0;
        int mins  = 0;
        int hours = 0;

        if( !wScheduleEntry.isregularstop(entry) ) {
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "no regular stop in schedule entry %d: GO", scheduleIdx );
          go = True;
        }
        else {
          if( timeprocessing == wSchedule.time_relative ) {
            modeltime = modeltime - data->scheduletime;
            modelminutes = modeltime / 60;
            mins  = modelminutes % 60;
            hours = modelminutes / 60;
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4203, "using relative time: modeltime=%d", modeltime );
          }
          else {
            ltm = localtime( &modeltime );
            hours = ltm->tm_hour;
            mins  = ltm->tm_min;
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4203, "using real time: modeltime=%d", modeltime );
          }

          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4203, "modeltime %02d:%02d (%ld)",
              hours, mins, modeltime );

          scheduleminutes = wScheduleEntry.gethour(entry) * 60 + wScheduleEntry.getminute(entry);

          if(timeprocessing == wSchedule.time_hourly ) {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4203, "using hourly timing" );
            /* processing hourly timing */
            modelminutes = mins;
            if( hours < fromhour || tohour < hours ) {
              TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4203,
                  "current hour, %d, is not in the hourly range from %d to %d", hours, fromhour, tohour );
              scheduleminutes += 60;
            }
            else if( modelminutes > scheduleminutes && modelminutes - scheduleminutes > timeframe ) {
              TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4203,
                  "diff between schedule[%d] and model[%d] time is bigger then the allowed frame of %d; force wait for next hour...",
                  scheduleminutes, modelminutes, timeframe );
              scheduleminutes += 60;
            }
          }
          else {
            modelminutes = hours * 60 + mins;
          }


          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4203,
              "check departure time schedule=%d model=%d index=%d", scheduleminutes, modelminutes, scheduleIdx );

          /* compare clock with departure time */
          if( scheduleminutes <= modelminutes ) {
            if( (modelminutes - scheduleminutes) <= maxdelay ) {
              TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4203,
                  "train must leave now %d <= %d and is delayed by %d minutes", scheduleminutes, modelminutes, modelminutes - scheduleminutes );
              go = True;
            }
            else {
              TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4203,
                  "train exceeded the max.(%d) delay time: %d", maxdelay, modelminutes - scheduleminutes );
            }
          }
          else {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4203,
                "train must wait %d > %d", scheduleminutes, modelminutes );
          }

        }

        break;
      }
      idx++;
      entry = wSchedule.nextscentry( schedule, entry );
    };

  }
  else
    go = True;

  return go;
}

Boolean isScheduleEnd( iILcDriverInt inst ) {
  iOLcDriverData data = Data(inst);
  iONode sc = data->model->getSchedule( data->model, data->schedule );
  if( sc != NULL ) {
    int nrEntries = 0;
    iONode scEntry = wSchedule.getscentry(sc);
    while( scEntry != NULL ) {
      nrEntries++;
      scEntry = wSchedule.nextscentry(sc, scEntry);
    };
    if( data->scheduleIdx >= nrEntries ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4203,
          "end of schedule[%s] detected; entries=%d index=%d", data->schedule, nrEntries, data->scheduleIdx);
      data->scheduleended = True; /* flag for signaling the tour for the next schedule */
      return True;
    }
  }
  return False;
}



