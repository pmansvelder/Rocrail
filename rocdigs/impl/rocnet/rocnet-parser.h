/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef ROCNETPARSER_H_
#define ROCNETPARSER_H_

#include "rocdigs/impl/rocnet/rocnet-const.h"
#include "rocs/public/rocs.h"

Boolean rocnetIsThis( iOrocNet rocnet, byte* rn );
const char* rocnetGetProtocolStr(byte prot);

byte* rocnetParseMobile( iOrocNet rocnet, byte* rn );
byte* rocnetParseGeneral( iOrocNet rocnet, byte* rn );
byte* rocnetParseOutput( iOrocNet rocnet, byte* rn );




#endif /* ROCNETPARSER_H_ */
