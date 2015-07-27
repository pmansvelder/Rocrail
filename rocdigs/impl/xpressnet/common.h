 /*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/


#ifndef COMMON_H_
#define COMMON_H_
#include "rocs/public/mem.h"


Boolean isChecksumOK(byte* in);
int makeChecksum(byte* out);
#endif /* COMMON_H_ */
