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



void statusTimer( iILcDriverInt inst, Boolean reverse ) {
  iOLcDriverData data = Data(inst);
  Boolean oppwait = True;
  data->opponly   = False;

  if( data->timer == -1 ) {
    /* handle manual operated signal */
    Boolean wait = data->curBlock->wait(data->curBlock, data->loc, reverse, &oppwait );
    if( !wait || !oppwait ) {
      data->timer = 0;
      if( wait && !oppwait )
        data->opponly = True;
    }
  }

  if( data->timer == 0 || !data->run || data->reqstop ) {

    if( data->reqstop ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201,"stop requested");
      data->reqstop = False;
      data->run = False;
      data->warningnodestfound = False;
    }

    data->state = LC_IDLE;
    data->loc->setMode(data->loc, wLoc.mode_idle, "");
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Setting state for \"%s\" from LC_TIMER to LC_IDLE.",
                   data->loc->getId( data->loc ) );

    if( data->next1Block != NULL )
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201, "next1Block for [%s] is [%s]",
                     data->loc->getId( data->loc ), data->next1Block->base.id(data->next1Block) );
    if( data->next2Block != NULL )
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201, "next2Block for [%s] is [%s]",
                     data->loc->getId( data->loc ), data->next2Block->base.id(data->next2Block) );
    if( data->next3Block != NULL )
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 4201, "next3Block for [%s] is [%s]",
                     data->loc->getId( data->loc ), data->next3Block->base.id(data->next3Block) );

  }
  else {
    if( data->timer > 0 )
      data->timer--;
  }
}
