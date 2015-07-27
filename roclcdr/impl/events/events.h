/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/


void eventFree( iOLcDriver inst, const char* blockId, iIBlockBase block, Boolean curBlockEvent, Boolean dstBlockEvent );
void eventEnter( iOLcDriver inst, const char* blockId, iIBlockBase block, Boolean curBlockEvent, Boolean dstBlockEvent );
void eventPre2In( iOLcDriver inst, const char* blockId, Boolean curBlockEvent, Boolean dstBlockEvent );
void eventIn( iOLcDriver inst, const char* blockId, iIBlockBase block, Boolean curBlockEvent, Boolean dstBlockEvent, Boolean shortIn );
void eventExit( iOLcDriver inst, const char* blockId, Boolean curBlockEvent, Boolean dstBlockEvent );
void freePrevBlock(iOLcDriver inst, iIBlockBase block);
