/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 

 Some parts are copied from the DDL project of Torsten Vogt: http://www.vogt-it.com/OpenSource/DDL

 All rights reserved.
*/
#ifndef __IMPL_DDX_QUEUE_H__
#define __IMPL_DDX_QUEUE_H__


#define PKTSIZE     60

#define QEMPTY      -1
#define QNOVALIDPKT 0
#define QM1LOCOPKT  1
#define QM2LOCOPKT  2
#define QM2FXPKT    3
#define QM1FUNCPKT  4
#define QM1SOLEPKT  5
#define QNBLOCOPKT  6
#define QNBACCPKT   7

int  queue_init();
void queue_add(int addr, char *packet, int packet_type, int packet_size);
int  queue_get(int *addr, char *packet, int *packet_size);

#endif
