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



void statusWait4Event( iILcDriverInt inst ) {
  iOLcDriverData data = Data(inst);
  Boolean oppwait = True;

  if( data->next1Block != NULL ) {
    if( data->next2Block == NULL ) {
      if( data->loc->isCheck2In( data->loc ) &&
          !data->next1Block->wait( data->next1Block, data->loc, !data->next1RouteFromTo, &oppwait ) &&
          data->run && !data->reqstop )
      {
        /* set step back to ENTER? may be a possible destination block did come free... */
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201,
            "Setting state for [%s] from LC_WAIT4EVENT to LC_RE_ENTERBLOCK. (check for free block)",
            data->loc->getId( data->loc ) );
        data->state = LC_RE_ENTERBLOCK;
        data->reentertimer = wLoc.getpriority( data->loc->base.properties( data->loc ) );
      }
    }
    else {
      if( !data->next2Route->isSet(data->next2Route) ) {
        if( !data->gomanual && !data->slowdown4route ) {
          /* set velocity to v_mid */
          iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
          if( data->loc->compareVhint( data->loc, wLoc.mid) == -1 ) {
            wLoc.setV_hint( cmd, wLoc.mid );
            wLoc.setdir( cmd, wLoc.isdir( data->loc->base.properties( data->loc ) ) );
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201, "Slow down for **not set** route running %s", data->loc->getId( data->loc ) );
            data->loc->cmd( data->loc, cmd );
          }
          data->slowdown4route = True;
        }
      }
      else if(data->slowdown4route) {
        if( !data->gomanual && !data->didReduceSpeedAtEnter ) {
          /* set the velocity back */
          iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
          int maxkmh = 0;
          wLoc.setV_hint( cmd, getBlockV_hint(inst, data->next1Block, False, data->next1Route, !data->next1RouteFromTo, &maxkmh ) );
          wLoc.setdir( cmd, wLoc.isdir( data->loc->base.properties( data->loc ) ) );
          wLoc.setV_maxkmh(cmd, maxkmh);
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201, "Restore normale velocity running %s", data->loc->getId( data->loc ) );
          data->loc->cmd( data->loc, cmd );
          data->slowdown4route = False;
        }
      }
    }
  }
}
