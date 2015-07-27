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



void statusPause( iILcDriverInt inst, Boolean reverse ) {
  iOLcDriverData data = Data(inst);
  Boolean oppwait = True;

  if( data->pause == -1 ) {
    /* handle manual operated signal */
    if( !data->curBlock->wait(data->curBlock, data->loc, reverse, &oppwait ) ) {
      data->pause = 0;
      data->state = LC_IDLE;
      data->loc->setMode(data->loc, wLoc.mode_idle, "");
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201,
                     "Setting state for \"%s\" from LC_PAUSE to LC_IDLE for manual signal.",
                     data->loc->getId( data->loc ) );
    }
  }
  else if( data->pause == 0 ) {
    data->state = LC_IDLE;
    data->loc->setMode(data->loc, wLoc.mode_idle, "");
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201,
                   "Setting state for \"%s\" from LC_PAUSE to LC_IDLE.",
                   data->loc->getId( data->loc ) );
  }
  else if( data->pause > 0 )
    data->pause--;
}
