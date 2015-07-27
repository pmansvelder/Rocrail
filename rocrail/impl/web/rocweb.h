/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef WEBME_H_
#define WEBME_H_

#define SERVER_KEY "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

Boolean rocWeb( iOPClient inst, const char* str );
Boolean rocWebSocket( iOPClient inst, iONode event, char** cmd );
Boolean rocWebSocketClose( iOPClient inst );

#endif /*WEBME_H_*/
