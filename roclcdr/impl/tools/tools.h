/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#define GREEN_ASPECT  0
#define YELLOW_ASPECT 1
#define RED_ASPECT    2
#define WHITE_ASPECT  3
#define DEFAULT_ASPECT  4

void setCrossingblockSignals(iOLcDriver inst, iORoute route, int aspect, Boolean routefromto);

void unlockBlockGroup( iOLcDriver inst, const char* group);
void reserveSecondNextBlock( iOLcDriver inst, const char* gotoBlock, iIBlockBase fromBlock,
    iORoute fromRoute, iIBlockBase* toBlock, iORoute* toRoute, Boolean reverse, Boolean swapNext1Route );
void listBlocks(iOLcDriver inst);
void resetNext2( iOLcDriver inst, Boolean unLock );


Boolean initializeDestination( iOLcDriver inst, iIBlockBase block, iORoute street, iIBlockBase curBlock, Boolean dir, int indelay );
Boolean initializeGroup( iOLcDriver inst, iIBlockBase block, iIBlockBase curBlock );
const char* getBlockV_hint( iILcDriverInt inst, iIBlockBase block, Boolean onexit, iORoute street, Boolean reverse, int* maxkmh );


void resetSignals(iOLcDriver inst );
Boolean setSignals(iOLcDriver inst, Boolean onEnter );

void checkScheduleActions( iILcDriverInt inst, int state);
Boolean checkScheduleEntryActions( iILcDriverInt inst, int index, Boolean checkSwapOnly );

Boolean checkScheduleTime( iILcDriverInt inst, const char* scheduleID, int scheduleIdx );
Boolean isScheduleEnd( iILcDriverInt inst );
