/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/


#include "rocs/public/str.h"
#include "rocs/public/socket.h"

#if defined __linux__ || defined __APPLE__
#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

static char addressBuffer[INET_ADDRSTRLEN];

const char* getLocalIP(void) {
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa ->ifa_addr->sa_family==AF_INET) {
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if( StrOp.len(addressBuffer) > 0 && !StrOp.equals(addressBuffer, "127.0.0.1") ) {
              if (ifAddrStruct!=NULL)
                freeifaddrs(ifAddrStruct);
              return addressBuffer;
            }
        }
    }
    if (ifAddrStruct!=NULL)
      freeifaddrs(ifAddrStruct);
    return SocketOp.gethostaddr();
}

#else
const char* getLocalIP(void) {
  return SocketOp.gethostaddr();
}
#endif
