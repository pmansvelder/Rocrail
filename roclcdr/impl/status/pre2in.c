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



void statusPre2In( iILcDriverInt inst ) {
  iOLcDriverData data = Data(inst);

  /* set velocitiy to V_min if the train has to wait */
  if( data->next2Block == NULL ) {
    Boolean bbt = wLoc.isusebbt(data->loc->base.properties( data->loc )) && data->next1Block->allowBBT(data->next1Block);
    if( !data->gomanual && !bbt ) {
      iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setV_hint( cmd, wLoc.min );
      wLoc.setdir( cmd, wLoc.isdir( data->loc->base.properties( data->loc ) ) );
      data->loc->cmd( data->loc, cmd );
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201,
                     "Setting velocity for \"%s\" to V_Min",
                     data->loc->getId( data->loc ) );
    }
  }
  data->state = LC_WAIT4EVENT;
  data->eventTimeout = 0;
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201,
                 "Setting state for \"%s\" from LC_PRE2INBLOCK to LC_WAIT4EVENT.",
                 data->loc->getId( data->loc ) );
}
