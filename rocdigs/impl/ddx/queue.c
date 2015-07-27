/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 Some parts are copied from the DDL project of Torsten Vogt: http://www.vogt-it.com/OpenSource/DDL

 All rights reserved.
*/
#include "queue.h"

#include "rocs/public/trace.h"
#include "rocs/public/mutex.h"
#include "rocs/public/mem.h"

#include <stdlib.h>

#define QSIZE 2000

iOMutex queue_mutex;   /* mutex to synchronize queue inserts */

static int queue_initialized = 0;
static int out=0, in=0;

typedef struct _tQData {
   int  packet_type;
   int  packet_size;
   char packet[PKTSIZE];
   int  addr;
} tQData ;
tQData QData[QSIZE];

int queue_init() {

   int error,i;
 
   queue_mutex = MutexOp.inst(NULL, True);

   for (i=0; i<QSIZE; i++) {
      QData[i].packet_type=QNOVALIDPKT;
      QData[i].addr=0;
      MemOp.set(QData[i].packet, 0, PKTSIZE);
   }
   in  = 0;
   out = 0;

   queue_initialized = 1;
   
   TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "Queue initialized." );
   return 0;
}       

int queue_empty() {
   return (in==out);
}

void queue_add(int addr, char *packet, int packet_type, int packet_size) {

   if (!queue_initialized) queue_init();
	
   MutexOp.wait(queue_mutex);
   MemOp.set(QData[in].packet,0,PKTSIZE);
   MemOp.copy(QData[in].packet,packet,packet_size);
   QData[in].packet_type=packet_type;
   QData[in].packet_size=packet_size;
   QData[in].addr=addr;
   in++;
   if (in==QSIZE) in=0;
   MutexOp.post(queue_mutex);
}

int queue_get(int *addr, char *packet, int *packet_size) {

   int rtc;

   if (!queue_initialized || queue_empty()) return QEMPTY;

   MemOp.copy(packet,QData[out].packet,PKTSIZE);   
   rtc=QData[out].packet_type;
   *packet_size=QData[out].packet_size;
   *addr=QData[out].addr;
   QData[out].packet_type=QNOVALIDPKT;
   out++;
   if (out==QSIZE) out=0;
   return rtc;   
}
