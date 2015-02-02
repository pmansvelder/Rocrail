/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2015 Rob Versluis, Rocrail.net

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "rocs/impl/json_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/node.h"
#include "rocs/public/strtok.h"
#include "rocs/public/trace.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
}

static const char* __name( void ) {
  return name;
}

static unsigned char* __serialize( void* inst, long* size ) {
  return NULL;
}

static void __deserialize( void* inst,unsigned char* bytestream ) {
  return;
}

static char* __toString( void* inst ) {
  return NULL;
}

static int __count( void ) {
  return instCnt;
}

static struct OBase* __clone( void* inst ) {
  return NULL;
}

static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}

static void* __properties( void* inst ) {
  return NULL;
}

static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/** ----- OJSon ----- */


/** Convert XML to a JSon string. */
static char* _toJSon( iONode xml ) {
  return NULL;
}


/*
{
    "name": "Philips hue",
    "zigbeechannel": 15,
    "mac": "00:17:88:00:00:00",
    "dhcp": true,
    "ipaddress": "192.168.1.7",
    "netmask": "255.255.255.0",
    "gateway": "192.168.1.1",
    "proxyaddress": "none",
    "proxyport": 0,
    "UTC": "2014-07-17T09:27:35",
    "localtime": "2014-07-17T11:27:35",
    "timezone": "Europe/Madrid",
    "whitelist": {
        "ffffffffe0341b1b376a2389376a2389": {
            "last use date": "2014-07-17T07:21:38",
            "create date": "2014-04-08T08:55:10",
            "name": "PhilipsHueAndroidApp#TCT ALCATEL ONE TOU"
        },
        "pAtwdCV8NZId25Gk": {
            "last use date": "2014-05-07T18:28:29",
            "create date": "2014-04-09T17:29:16",
            "name": "MyApplication"
        },
        "gDN3IaPYSYNPWa2H": {
            "last use date": "2014-05-07T09:15:21",
            "create date": "2014-05-07T09:14:38",
            "name": "iPhone Web 1"
        }
    },
    "swversion": "01012917",
    "apiversion": "1.3.0",
    "swupdate": {
        "updatestate": 0,
        "url": "",
        "text": "",
        "notify": false
    },
    "linkbutton": false,
    "portalservices": false,
    "portalconnection": "connected",
    "portalstate": {
        "signedon": true,
        "incoming": false,
        "outgoing": true,
        "communication": "disconnected"
    }
}
*/

static iONode __parseAttribute( const char* s, iONode xml, int* mainIdx) {
  iONode child = NULL;
  int i = 0;
  char* attName = StrOp.dup(s);
  char* attVal  = StrOp.dup(s);
  int attNameIdx = 0;
  int attValIdx  = 0;

  iOStrTok tok = StrTokOp.inst( s, ':');

  attName[0] = '\0';
  attVal [0] = '\0';

  if( StrTokOp.hasMoreTokens(tok) ) {
    const char* aname = StrTokOp.nextToken(tok);
    int len = StrOp.len(aname);
    Boolean startName = False;
    for( i = 0; i < len; i++ ) {
      if( aname[i] == '\"' && !startName ) {
        startName = True;
      }
      else if( aname[i] != '\"' && startName ) {
        attName[attNameIdx] = aname[i];
        attNameIdx++;
        attName[attNameIdx] = '\0';
      }
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "attribute name [%s]", attName);
  }

  if( StrTokOp.hasMoreTokens(tok) ) {
    const char* aval = StrTokOp.nextToken(tok);
    int len = StrOp.len(aval);
    Boolean startVal = False;
    for( i = 0; i < len; i++ ) {
      if( aval[i] != ' ' && !startVal ) {
        if( aval[i] == '{' ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "new child node <%s>", attName);
          child = NodeOp.inst(attName, xml, ELEMENT_NODE);
          iONode childchild = __parseAttribute(aval, child, mainIdx);
          if( childchild != NULL ) {
            NodeOp.addChild( child, childchild );
          }
          break;
        }
        else {
          startVal = True;
        }
      }

      if( aval[i] != '\"' && aval[i] != '}' && startVal ) {
        attVal[attValIdx] = aval[i];
        attValIdx++;
        attVal[attValIdx] = '\0';
      }
    }
  }

  if( child == NULL && StrOp.len(attName) > 0 ) {
    NodeOp.setStr( xml, attName, attVal );
  }

  return child;
}

static iONode _toXML( const char* json ) {
  iONode xml = NodeOp.inst( "json", NULL, ELEMENT_NODE );
  int len = StrOp.len(json);
  int i = 0;

  for( i = 0; i < len; i++ ) {
    if( json[i] == '{' ) {
      /* start op node contents */
      iOStrTok tok = StrTokOp.inst( json+i+1, ',');
      while( StrTokOp.hasMoreTokens(tok) ) {
        const char* s = StrTokOp.nextToken(tok);
        iONode child = __parseAttribute(s, xml, &i);
        if( child != NULL ) {
          NodeOp.addChild( xml, child );
        }
      }
    }
  }

  return xml;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/json.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
