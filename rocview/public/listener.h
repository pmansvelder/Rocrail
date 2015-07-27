/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 All rights reserved.
*/

#ifndef ROCVIEW_LISTENER_H_
#define ROCVIEW_LISTENER_H_
#include "rocs/public/trace.h"


class Listener {
public:
  virtual void handleEvent( iONode node ){;}
};


#endif /* ROCVIEW_LISTENER_H_ */
