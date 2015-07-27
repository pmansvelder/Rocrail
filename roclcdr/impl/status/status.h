/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

void statusPause( iILcDriverInt inst, Boolean reverse );
void statusIdle( iILcDriverInt inst, Boolean reverse );
void statusFindDest( iILcDriverInt inst );
void statusInitDest( iILcDriverInt inst );
void statusCheckRoute( iILcDriverInt inst );
void statusPre2Go( iILcDriverInt inst );
void statusGo( iILcDriverInt inst );
void statusExit( iILcDriverInt inst );
void statusOut( iILcDriverInt inst );
void statusEnter( iILcDriverInt inst, Boolean re_enter );
void statusPre2In( iILcDriverInt inst );
void statusIn( iILcDriverInt inst );
void statusWait( iILcDriverInt inst, Boolean reverse );
void statusTimer( iILcDriverInt inst, Boolean reverse );
void statusWait4Event( iILcDriverInt inst );

