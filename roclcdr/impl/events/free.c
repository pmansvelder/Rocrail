/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 All rights reserved.
*/

#include "roclcdr/impl/lcdriver_impl.h"

#include "roclcdr/impl/tools/tools.h"
#include "roclcdr/impl/events/events.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"



#include "rocrail/public/model.h"

#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Block.h"

void eventFree( iOLcDriver inst, const char* blockId, iIBlockBase block, Boolean curBlockEvent, Boolean dstBlockEvent ) {
  iOLcDriverData data = Data(inst);

  if( wLoc.isfreeblockonenter(data->loc->base.properties(data->loc)) && data->next1Block->isFreeBlockOnEnter(data->next1Block) )
  {
    Boolean dontcare = False;
    if( data->state == LC_GO || data->state == LC_PRE2GO )
      dontcare = True;

    if( (dstBlockEvent && data->state == LC_EXITBLOCK) || (dstBlockEvent && data->state == LC_OUTBLOCK) || (dstBlockEvent && dontcare) )
    {
      if( !data->didFree ) {
        TraceOp.trc(name, TRCLEVEL_USER1, __LINE__, 4201,
            "Free previous block on free for [%s] in [%s] with state [%d]", data->loc->getId(data->loc), blockId, data->state);
        freePrevBlock(inst, block);
        data->didFree = True;
      }
    }
  }

}
