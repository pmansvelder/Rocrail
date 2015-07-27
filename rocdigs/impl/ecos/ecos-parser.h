/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef ECOS_PARSER_H_
#define ECOS_PARSER_H_

#include "rocs/public/node.h"
#include "rocs/public/socket.h"

#define REPLY_TYPE_REPLY 0
#define REPLY_TYPE_EVENT 1
 

iONode ecos_reader( iOSocket socket );
iONode ecos_parser( const char* reply );


#endif /*ECOS_PARSER_H_*/
