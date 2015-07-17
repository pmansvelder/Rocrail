/*
 Rocrail - Model Railroad Software

 Copyright (c) 2002 Robert Jan Versluis, Rocrail.net

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


/**  */
static char* __toJSonString( iONode xml, int level ) {
  int      i = 0;
  char*  str = NULL;
  char*  fmt = NULL;

  if( level == 0 )
    fmt = StrOp.fmt( "{" );
  else
    fmt = StrOp.fmt( "\"%s\": {", NodeOp.getName(xml) );
  str = StrOp.cat( str, fmt );
  StrOp.free( fmt );
  for( i = 0; i < NodeOp.getAttrCnt(xml); i++ ) {
    iOAttr a = NodeOp.getAttr(xml,i);
    long len;
    fmt = StrOp.fmt( "\"%s\": \"%s\"", AttrOp.getName(a), AttrOp.getVal(a) );
    str = StrOp.cat( str, fmt );
    if( i + 1 < NodeOp.getAttrCnt(xml) || NodeOp.getChildCnt(xml) > 0)
      str = StrOp.cat( str, "," );
    str = StrOp.cat( str, " " );
    StrOp.free( fmt );
  }

  if( NodeOp.getChildCnt(xml) == 0 ) {
    str = StrOp.cat( str, "} " );
    return str;
  }

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "child count = %d", NodeOp.getChildCnt(xml));
  for( i = 0; i < NodeOp.getChildCnt(xml); i++ ) {
    iONode child = NodeOp.getChild(xml,i);
    fmt = __toJSonString( child, level + 1 );
    str = StrOp.cat( str, fmt );
    StrOp.free( fmt );
  }

  fmt = StrOp.fmt( "} " );
  str = StrOp.cat( str, fmt );
  StrOp.free( fmt );

  return str;
}


static char* _toJSon( iONode xml ) {
  return __toJSonString(xml, 0);
}

/*
{
    "name": "xSpooler",
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
    "swversion": "01012917",
    "apiversion": "1.3.0",
    "swupdate": {
        "updatestate": 0,
        "url": "",
        "text": "",
        "notify": false
    }
}



[
  {"success":{"/lights/1/state/bri":200}},
  {"success":{"/lights/1/state/on":true}},
  {"success":{"/lights/1/state/hue":50000}}
]

{"deviceid":["45AF34","543636","34AFBE"]}

{
  "7": {"name": "Hue Lamp 7"},
  "8": {"name": "Hue Lamp 8"},
  "lastscan": "2012-10-29T12:00:00"
}


*/
static iONode __parseNode(const char* nodeName, const char* json, int* idx) {
  iONode xml = NULL;
  int len = StrOp.len(json);
  Boolean startName = False;
  Boolean endName   = False;
  Boolean pendingVal = False;
  Boolean startVal = False;
  Boolean endVal   = False;
  Boolean strVal   = False;
  Boolean startArray = False;
  char* attName = StrOp.dup(json);
  char* attVal  = StrOp.dup(json);
  int attNameIdx = 0;
  int attValIdx  = 0;
  attName[0] = '\0';
  attVal [0] = '\0';

  if( nodeName == NULL ) {

  }
  else {
    xml = NodeOp.inst( nodeName, NULL, ELEMENT_NODE );
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "new node <%s>", nodeName );
  }

  while( *idx < len ) {
    if( json[*idx] == '}' ) {
      if( pendingVal && nodeName != NULL ) {
        if( StrOp.len(attName) > 0 ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "new attribute %s=\"%s\"", attName, attVal );
          NodeOp.setStr( xml, attName, attVal );
        }
        else {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "value without name \"%s\"", attVal );
        }
      }
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "end node <%s>", nodeName );
      return xml;
    }
    else if( json[*idx] == '\"' && !startName ) {
      startName = True;
    }
    else if( startName && !endName ) {
      if( json[*idx] == '\"' ) {
        endName = True;
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "new attribute name \"%s\"", attName );
      }
      else {
        attName[attNameIdx] = json[*idx];
        attNameIdx++;
        attName[attNameIdx] = '\0';
      }
    }
    else if( endName ) {
      if( json[*idx] == ':' ) {
        pendingVal = True;
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "pending value..." );
      }
      else if( pendingVal ) {
        if( json[*idx] == '{' ) {
          iONode child = __parseNode(attName, json, idx);
          if( child != NULL ) {
            if( xml == NULL )
              xml = child;
            else
              NodeOp.addChild(xml, child);
          }
          startName = False;
          endName   = False;
          pendingVal = False;
          startVal = False;
          endVal   = False;
          strVal   = False;
          startArray = False;
          attNameIdx = 0;
          attValIdx  = 0;
          attName[0] = '\0';
          attVal [0] = '\0';
        }
        else if( json[*idx] == '[' && !startVal && !endVal) {
          startArray = True;
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "start array..." );
        }
        else if( json[*idx] == ']' && startArray) {
          startArray = False;
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "end array..." );
        }
        else if( json[*idx] == '\"' && !startVal && !endVal) {
          startVal = True;
          strVal   = True;
        }
        else if( json[*idx] > ' ' && !startVal && !endVal) {
          startVal = True;
          attVal[attValIdx] = json[*idx];
          attValIdx++;
          attVal[attValIdx] = '\0';
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "value \"%s\"...", attVal );
        }
        else if( startVal && !endVal ) {
          if( json[*idx] == '\"' || (!strVal && json[*idx] == '}' ) || (!strVal && json[*idx] == ',' ) ) {
            endVal = True;

            if( startArray ) {
              startVal = False;
              endVal   = False;
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "array value \"%s\"...", attVal );
            }
            else {
              if( StrOp.len(attName) > 0 ) {
                TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "new attribute %s=\"%s\"", attName, attVal );
                NodeOp.setStr( xml, attName, attVal );
              }
              else {
                TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "value without name \"%s\"", attVal );
              }
              startName = False;
              endName   = False;
              pendingVal = False;
              startVal = False;
              endVal   = False;
              strVal   = False;
              startArray = False;
              attNameIdx = 0;
              attValIdx  = 0;
              attName[0] = '\0';
              attVal [0] = '\0';
            }
          }
          else {
            attVal[attValIdx] = json[*idx];
            attValIdx++;
            attVal[attValIdx] = '\0';
            //TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "def value \"%s\"...", attVal );

            if( !strVal && (StrOp.equals("true", attVal) || StrOp.equals("false", attVal) || StrOp.equals("null", attVal) ) ) {
              endVal = True;
              if( StrOp.len(attName) > 0 ) {
                TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "new attribute %s=\"%s\"", attName, attVal );
                NodeOp.setStr( xml, attName, attVal );
              }
              startName = False;
              endName   = False;
              pendingVal = False;
              startVal = False;
              endVal   = False;
              strVal   = False;
              startArray = False;
              attNameIdx = 0;
              attValIdx  = 0;
              attName[0] = '\0';
              attVal [0] = '\0';
            }
          }
        }
        else {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "drop -\"%c\"-...", json[*idx] );
        }
      }
    }
    *idx = *idx + 1;
    if( *idx >= len ) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "idx == len (%d, %d) ", *idx, len );
    }
  }

  return xml;
}


static iONode _toXML( const char* json ) {
  iONode xml = NULL;
  int len = StrOp.len(json);
  int idx = 0;
  int i = 0;

  for( i = 0; i < len; i++ ) {

    if( json[i] == '[' ) {
      xml = NodeOp.inst( "json", NULL, ELEMENT_NODE );
      iONode child = NULL;
      idx = i + 1;
      do {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "***** %.20s", json+idx);
        child = __parseNode(NULL, json, &idx);
        if( child != NULL ) {
          NodeOp.addChild(xml, child);
          const char* p = StrOp.findc(json+idx, '{');
          if( p != NULL ) {
            idx = p - json;
          }
        }
      } while( child != NULL );
      break;
    }

    if( json[i] == '{' ) {
      /* root node */
      idx = i + 1;
      xml = __parseNode("json", json, &idx);
      break;
    }
  }

  return xml;
}



/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/json.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
