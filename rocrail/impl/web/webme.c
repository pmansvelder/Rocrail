/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

 


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
#include "rocrail/impl/pclient_impl.h"

#include "rocrail/impl/web/webme.h"
#include "rocrail/impl/web/web.h"

#include "rocrail/public/model.h"
#include "rocrail/public/app.h"

#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Global.h"
#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/WebClient.h"

#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/socket.h"
#include "rocs/public/file.h"
#include "rocs/public/mime64.h"
#include "rocs/public/system.h"



static const char* ROCWEB_INDEX    = "index.html";
static const char* ROCWEB_JS       = "rocweb.js";
static const char* ROCWEBWORKER_JS = "rocwebworker.js";
static const char* ROCWEB_CSS      = "rocweb.css";
static const char* ROCWEB_LOGO     = "logo.png";

static void __getFile(iOPClient inst, const char* fname) {
  iOPClientData data = Data(inst);

  char* webfile = StrOp.fmt("%s/%s", wWebClient.getwebpath(data->ini), fname);

  if( FileOp.exist( webfile ) ) {
    long size = FileOp.fileSize( webfile );
    char* html = allocMem( size + 1 );
    iOFile f = FileOp.inst( webfile, OPEN_READONLY );
    if( f != NULL ) {
      Boolean ok = True;
      FileOp.read( f, html, size );
      FileOp.base.del( f );
      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "write %s %d", fname, size );
      if(ok) ok=SocketOp.fmt( data->socket, "HTTP/1.1 200 OK\r\n" );
      if(ok) ok=SocketOp.fmt( data->socket, "Connection: close\r\n" );
      if(ok) ok=SocketOp.fmt( data->socket, "Content-type: text/html\r\n\r\n" );
      if(ok) ok=SocketOp.write( data->socket, (char*)html, size );
    }
    freeMem(html);
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "file [%s] not found", webfile );
    SocketOp.fmt( data->socket, "HTTP/1.1 404 Not found\r\n\r\n" );
  }

  StrOp.free(webfile);
}


static char* __rotateSVG(const char* svgStr, const char* ori) {
  char* svgNew = NULL;
  iODoc doc = DocOp.parse( svgStr );
  if( doc == NULL || DocOp.getRootNode( doc ) == NULL) {
    TraceOp.trc( "svg", TRCLEVEL_EXCEPTION, __LINE__, 9999, "svg not parsed\n %60.60s", svgStr );
    return NULL;
  }
  iONode svg = DocOp.getRootNode( doc );
  DocOp.base.del(doc);

  /* If this is missing the browser won't show the svg:
   * <svg xmlns="http://www.w3.org/2000/svg" ...> */
  if( NodeOp.getStr(svg, "xmlns", NULL) == NULL) {
    NodeOp.setStr(svg, "xmlns", "http://www.w3.org/2000/svg");
  }

  if( !StrOp.equals(ori, "west") ) {
    iONode g = NodeOp.findNode(svg, "g");
    /* ToDo: Rotate with the orinr the SVG.
     * use the transform in the g element for rotating:
     * */
    /*
    <svg width="128" height="32" xmlns="http://www.w3.org/2000/svg">
      <!--<g transform="rotate(270, 64, 64)">-->
      <g transform="rotate(90, 16, 16)">
        <!-- Block -->
        <path stroke="rgb(0,0,0)" fill="none" d="M 0,3 L 127,3 L 127,28 L 0,28 z " />
        <path stroke="rgb(0,0,0)" fill="rgb(255,255,255)" d="M 1,4 L 126,4 L 126,27 L 1,27 z " />
        <!--path stroke="rgb(0,0,0)" fill="black" d="M 1,24 L 14,24 L 14,27 L 1,27 z " /-->
        <path stroke="rgb(0,0,0)" fill="none" d="M 3,23 L 7,23 z " />
        <path stroke="rgb(0,0,0)" fill="none" d="M 5,21 L 5,25 z " />
      </g>
    </svg>

     */
    if( g != NULL ) {
      int width  = NodeOp.getInt(svg, "width", 32);
      int height = NodeOp.getInt(svg, "height", 32);
      int center = 0;
      char* transform = NULL;
      int deg = 0;
      /* ToDo: Calculate the center of the rotation... */
      if( StrOp.equals(ori, "east") ) {
        deg = 180;
        center = width / 2;
      }
      else if( StrOp.equals(ori, "south") ) {
        deg = 90;
        center = height / 2;
        NodeOp.setInt(svg, "width", height);
        NodeOp.setInt(svg, "height", width);
      }
      else if( StrOp.equals(ori, "north") ) {
        deg = 270;
        center = width / 2;
        NodeOp.setInt(svg, "width", height);
        NodeOp.setInt(svg, "height", width);
      }
      transform = StrOp.fmt("rotate(%d, %d, %d)", deg, center, center);
      NodeOp.setStr(g, "transform", transform);
      StrOp.free(transform);
    }

  }

  svgNew = NodeOp.base.toString(svg);
  NodeOp.base.del(svg);
  return svgNew;
}

static void __getSVG(iOPClient inst, const char* fname) {
  iOPClientData data = Data(inst);
  iOStrTok tok = StrTokOp.inst( fname, '.' );

  const char* svgname = StrTokOp.nextToken( tok );
  const char* ori     = StrTokOp.nextToken( tok );
  /* Multiple theme support in the rocrail.ini. */
  char* svg = StrOp.fmt("%s/%s.svg", wWebClient.getsvgpath1(data->ini), svgname);
  if( !FileOp.exist( svg ) ) {
    StrOp.free(svg);
    svg = StrOp.fmt("%s/%s.svg", wWebClient.getsvgpath2(data->ini), svgname);
  }
  if( !FileOp.exist( svg ) ) {
    StrOp.free(svg);
    svg = StrOp.fmt("%s/%s.svg", wWebClient.getsvgpath3(data->ini), svgname);
  }
  if( !FileOp.exist( svg ) ) {
    StrOp.free(svg);
    svg = StrOp.fmt("%s/%s.svg", wWebClient.getsvgpath4(data->ini), svgname);
  }
  if( !FileOp.exist( svg ) ) {
    StrOp.free(svg);
    svg = StrOp.fmt("%s/%s.svg", wWebClient.getsvgpath5(data->ini), svgname);
  }

  if( FileOp.exist( svg ) ) {
    char* svgRotated = NULL;
    long size = FileOp.fileSize( svg );
    char* html = allocMem( size + 1 );
    iOFile f = FileOp.inst(svg, OPEN_READONLY );
    if( f != NULL ) {
      Boolean ok = True;
      FileOp.read( f, html, size );
      FileOp.base.del( f );
      svgRotated = __rotateSVG(html, ori);
      if(svgRotated != NULL ) {
        size = StrOp.len(svgRotated);
        TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "write %s (%s) %d", fname, svg, size );
        if(ok) ok=SocketOp.fmt( data->socket, "HTTP/1.1 200 OK\r\n" );
        if(ok) ok=SocketOp.fmt( data->socket, "Connection: close\r\n" );
        if(ok) ok=SocketOp.fmt( data->socket, "Content-type: image/%s\r\n\r\n", "svg+xml" );
        if(ok) ok=SocketOp.write( data->socket, (char*)svgRotated, size );
        StrOp.free(svgRotated);
      }
      else {
        TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "write %s (%s) %d", fname, svg, size );
        if(ok) ok=SocketOp.fmt( data->socket, "HTTP/1.1 200 OK\r\n" );
        if(ok) ok=SocketOp.fmt( data->socket, "Connection: close\r\n" );
        if(ok) ok=SocketOp.fmt( data->socket, "Content-type: image/%s\r\n\r\n", "svg+xml" );
        if(ok) ok=SocketOp.write( data->socket, (char*)html, size );
      }
    }
    freeMem(html);
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "image not found: %s", svg );
    SocketOp.fmt( data->socket, "HTTP/1.1 404 Not found\r\n\r\n" );
  }

  StrTokOp.base.del(tok);

  StrOp.free(svg);
}


static void __getImage(iOPClient inst, const char* fname, Boolean qualifiedPath) {
  iOPClientData data = Data(inst);

  char* png = NULL;

  if( !qualifiedPath )
    png = StrOp.fmt("%s/%s", wWebClient.getimgpath(data->ini), fname);
  else
    png = StrOp.dup(fname);

  if( FileOp.exist( png ) ) {
    long size = FileOp.fileSize( png );
    char* html = allocMem( size + 1 );
    iOFile f = FileOp.inst( png, OPEN_READONLY );
    if( f != NULL ) {
      Boolean ok = True;
      FileOp.read( f, html, size );
      FileOp.base.del( f );
      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "write %s %d", fname, size );
      if(ok) ok=SocketOp.fmt( data->socket, "HTTP/1.1 200 OK\r\n" );
      if(ok) ok=SocketOp.fmt( data->socket, "Connection: close\r\n" );
      if(ok) ok=SocketOp.fmt( data->socket, "Content-type: image/%s\r\n\r\n", "png" );
      if(ok) ok=SocketOp.write( data->socket, (char*)html, size );
    }
    freeMem(html);
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "image not found: %s", png );
    SocketOp.fmt( data->socket, "HTTP/1.1 404 Not found\r\n\r\n" );
  }
  StrOp.free(png);
}

static void __getModel(iOPClient inst) {
  iOPClientData data = Data(inst);
  Boolean ok = True;
  char* xml = NULL;
  int size = 0;
  iONode model = ModelOp.getModel( AppOp.getModel() );

  unsigned char* donkey = StrOp.strToByte(AppOp.getdonkey());
  char* decodedKey = SystemOp.decode(donkey, StrOp.len(AppOp.getdonkey())/2, AppOp.getdoneml());

  if( !SystemOp.isExpired(decodedKey, NULL, NULL, wGlobal.vmajor, wGlobal.vminor) ) {
    wPlan.setdonkey(model, True);
    TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "valid donation key: %s", wRocRail.getdoneml(AppOp.getIni()) );
  }
  else {
    wPlan.setdonkey(model, False);
    TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "no valid donation key found" );
  }

  freeMem(decodedKey);
  freeMem(donkey);

  xml = NodeOp.base.toString( model );
  size = StrOp.len(xml);
  TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "write model %d", size );
  if(ok) ok=SocketOp.fmt( data->socket, "HTTP/1.1 0 OK\r\n" );
  if(ok) ok=SocketOp.fmt( data->socket, "Connection: close\r\n" );
  if(ok) ok=SocketOp.fmt( data->socket, "Content-type: application/xml\r\n\r\n" );
  if(ok) ok=SocketOp.write( data->socket, xml, size );
  StrOp.free(xml);
}


/*
0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-------+-+-------------+-------------------------------+
|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
|N|V|V|V|       |S|             |   (if payload len==126/127)   |
| |1|2|3|       |K|             |                               |
+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
|     Extended payload length continued, if payload len == 127  |
+ - - - - - - - - - - - - - - - +-------------------------------+
|                               |Masking-key, if MASK set to 1  |
+-------------------------------+-------------------------------+
| Masking-key (continued)       |          Payload Data         |
+-------------------------------- - - - - - - - - - - - - - - - +
:                     Payload Data continued ...                :
+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
|                     Payload Data continued ...                |
+---------------------------------------------------------------+
The MASK bit simply tells whether the message is encoded. Messages from the client must be masked, so your server should expect this to be 1.
(In fact, section 5.1 of the spec says that your server must disconnect from a client if that client sends an unmasked message.)
When sending a frame back to the client, do not mask it and do not set the mask bit. We'll explain masking later. Note:
You have to mask messages even when using a secure socket.

The opcode field defines how to interpret the payload data: 0x0 for continuation, 0x1 for text (which is always encoded in UTF-8), 0x2 for binary,
and other so-called "control codes" that will be discussed later. In this version of WebSockets, 0x3 to 0x7 and 0xB to 0xF have no meaning.

The FIN bit tells whether this is the last message in a series. If it's 0, then the server will keep listening for more parts of the message;
otherwise, the server should consider the message delivered. More on this later.

====Decoding Payload Length====
To read the payload data, you must know when to stop reading. That's why the payload length is important to know. Unfortunately,
this is somewhat complicated. To read it, follow these steps:

1 Read bits 9-15 (inclusive) and interpret that as an unsigned integer. If it's 125 or less, then that's the length; you're done.
If it's 126, go to step 2. If it's 127, go to step 3.
2 Read the next 16 bits and interpret those as an unsigned integer. You're done.
3 Read the next 64 bits and interpret those as an unsigned integer. You're done.

===Reading and Unmasking the Data===

If the MASK bit was set (and it should be, for client-to-server messages), read the next 4 octets (32 bits); this is the masking key.
Once the payload length and masking key is decoded, you can go ahead and read that number of bytes from the socket. Let's call the data ENCODED,
and the key MASK. To get DECODED, loop through the octets (bytes a.k.a. characters for text data) of ENCODED and XOR the octet with
the (i modulo 4)th octet of MASK. In pseudo-code (that happens to be valid JavaScript):

var DECODED = "";
for (var i = 0; i < ENCODED.length; i++) {
    DECODED[i] = ENCODED[i] ^ MASK[i % 4];
}

 */
Boolean rocWebSocketME( iOPClient inst, iONode event, char** cmd ) {
  iOPClientData data = Data(inst);
  Boolean ok = True;
  char b[128];
  char bMask[10];
  int payload = 0;
  Boolean mask = False;

  if( SocketOp.isBroken( data->socket ) ) {
    TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "websocket down" );
    return True;
  }

  if( event != NULL ) {
    char* info = NodeOp.base.toString( event );
    int len = StrOp.len(info);
    char* b = allocMem(20 + len + 1);
    b[0] = 0x81;
    if( len+1 < 126 ) {
      b[1] = len+1;
      StrOp.copy(b+2, info);
      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "websocket: event[%d]=%.40s", len+1, b+2 );
      ok = SocketOp.write( data->socket, b, 2 + (b[1]&0x7F) );
    }
    else if( len+1 < 32767 ) {
      b[1] = 126;
      b[2] = ((len+1)/256) & 0x7F;
      b[3] = (len+1)%256;
      StrOp.copy(b+4, info);
      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "websocket: event[%d]=%.40s", len+1, b+4 );
      ok = SocketOp.write( data->socket, b, 4 + len+1 );
    }
    else {
      b[1] = 127;
      b[2] = ((len+1) >> 24) & 0x7F;
      b[3] = ((len+1) >> 16) & 0xFF;
      b[4] = ((len+1) >>  8) & 0xFF;
      b[5] = (len+1)  & 0xFF;
      StrOp.copy(b+4, info);
      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "websocket: event[%d]=%.40s", len+1, b+6 );
      ok = SocketOp.write( data->socket, b, 6 + len+1 );
    }
    freeMem(b);
    StrOp.free(info);
  }

  if( SocketOp.isBroken( data->socket ) ) {
    TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "websocket down" );
    return True;
  }

  if( !SocketOp.peek( data->socket, b, 1) ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "no work for rocWebSocketME" );
    ThreadOp.sleep(10);
    return False;
  }

  TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "work for rocWebSocketME" );

  ok = SocketOp.read( data->socket, b, 1 );
  TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "work for rocWebSocketME: 0x%02X", b );
  if(ok) {
    TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "websocket: fin=%s opcode=%d", b[0]&0x80?"true":"false", b[0]&0x0F );
    ok = SocketOp.read( data->socket, b, 1 );
    if(ok) {
      mask = b[0]&0x80 ? True:False;
      payload = b[0]&0x7F;
      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "websocket: mask=%s payload=%d", mask?"true":"false", payload );
      if( payload == 126 ) {
        ok = SocketOp.read( data->socket, b, 2 );
        if(ok) {
          payload = b[0]*256 + b[1];
          TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "websocket: payload=%d", payload );
        }
      }
      if( payload == 127 ) {
        ok = SocketOp.read( data->socket, b, 4 );
        if(ok) {
          payload = (b[0]<<24) + (b[1]<<16) + (b[2]<<8) + b[3] ;
          TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "websocket: payload=%d", payload );
        }
      }
      if(ok && mask ) {
        ok = SocketOp.read( data->socket, bMask, 4 );
      }
      if(ok) {
        char* buffer = allocMem(payload+1);
        ok = SocketOp.read( data->socket, buffer, payload );
        if(ok && mask) {
          char* decoded = allocMem(payload+1);
          int i = 0;
          for(i = 0; i < payload; i++) {
            decoded[i] = buffer[i] ^ bMask[i % 4];
          }
          MemOp.copy(buffer, decoded, payload);
          freeMem(decoded);
        }
        TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "websocket: message=%.80s", buffer );
        *cmd = StrOp.dup(buffer);

        freeMem(buffer);
      }
    }
  }

  TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "ready work for rocWebSocketME" );
  return !ok;
}


Boolean rocWebME( iOPClient inst, const char* str ) {
  char serverKey[128];
  char protocol[128];
  TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "work for rocWebME [%s]", str );
  
  if( inst != NULL ) {
    iOPClientData data = Data(inst);
    char l_str[1025] = {'\0'};

    TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "Reading rest of HTTP header... " );
    while( SocketOp.readln( data->socket, l_str ) && !SocketOp.isBroken( data->socket ) ) {
      if( l_str[0] == '\r' || l_str[0] == '\n' ) {
        break;
      }
      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "%s", l_str );

      if( StrOp.findi( l_str, "Connection: keep-alive" ) ) {

      }

      if( StrOp.findi( l_str, "Sec-WebSocket-Protocol:") ) {
        StrOp.replaceAll(l_str, '\n', '\0');
        StrOp.replaceAll(l_str, '\r', '\0');
        TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "websocket protocol=%s", l_str + 24 );
        StrOp.copy(protocol, l_str + 24);
      }

      if( StrOp.findi( l_str, "Sec-WebSocket-Key:") ) {
        byte dest[128];
        int dlen = 128;
        int rc = 0;
        char* clientkey = NULL;
        char* serverkey = NULL;
        char* sha1 = NULL;
        byte* mime64key = NULL;
        data->websocket = True;
        StrOp.replaceAll(l_str, '\n', '\0');
        StrOp.replaceAll(l_str, '\r', '\0');
        TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "websocket key=%s", l_str + 19 );
        rc = Mime64Op.decode(dest, &dlen, (const unsigned char*)(l_str+19), StrOp.len(l_str+19));
        TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "websocket rc=%d keylen=%d", rc, dlen );
        TraceOp.dump( name, TRCLEVEL_USER2, (const char*)dest, dlen );

        clientkey = StrOp.byteToStr(dest, dlen);
        TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "clientkey=%s", clientkey );
        serverkey = StrOp.cat(serverkey, l_str + 19);
        serverkey = StrOp.cat(serverkey, SERVER_KEY);
        TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "serverkey=%s", serverkey );

        sha1 = Mime64Op.sha1(serverkey);
        TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "sha1=%s", sha1 );

        mime64key = StrOp.strToByte(sha1);
        dlen = 128;
        rc = Mime64Op.encode(dest, &dlen, (const unsigned char*)mime64key, StrOp.len(sha1)/2);
        MemOp.copy(serverKey, dest, dlen);
        serverKey[dlen] = 0;
        TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "mime64=%s", serverKey );

        StrOp.free(clientkey);
        StrOp.free(serverkey);
        StrOp.free(sha1);
        freeMem(mime64key);

      }
    };

    if( data->websocket ) {
      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "accept the websocket..." );
      webSocketHeader( data->socket, serverKey, protocol );
      return False;
    }
    else {
      if( StrOp.find( str, "GET" ) && StrOp.find( str, " / " ) ) {
        __getFile( inst, ROCWEB_INDEX );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, "/rocweb.js" ) ) {
        __getFile( inst, ROCWEB_JS );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, "/rocwebworker.js" ) ) {
        __getFile( inst, ROCWEBWORKER_JS );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, "/rocweb.css" ) ) {
        __getFile( inst, ROCWEB_CSS );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, ".js" ) ) {
        char* jsfile = StrOp.dup( StrOp.find( str, " /" ) + 2 ) ;
        char* p = StrOp.find( jsfile, "HTTP" );

        if( p != NULL ) {
          p--;
          *p = '\0';
          __getFile( inst, jsfile );
        }
        StrOp.free( jsfile );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, ".css" ) ) {
        char* cssfile = StrOp.dup( StrOp.find( str, " /" ) + 2 ) ;
        char* p = StrOp.find( cssfile, "HTTP" );

        if( p != NULL ) {
          p--;
          *p = '\0';
          __getFile( inst, cssfile );
        }
        StrOp.free( cssfile );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, "/plan.xml" ) ) {
        Boolean ok = True;
        __getModel( inst );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, "/rocweb.xml?" ) ) {
        Boolean ok = True;
        TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "command: %s", str );
        if(ok) ok=SocketOp.fmt( data->socket, "HTTP/1.1 202 OK\r\n" );
        if(ok) ok=SocketOp.fmt( data->socket, "Content-type: application/xml\r\n\r\n" );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, "/update.xml" ) ) {
        Boolean ok = True;
        int randNumber = rand();
        int bigsleep = randNumber % 10000;

        TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "update...sleep=%d", bigsleep );
        /* ToDo: get an update event from queue. */
        ThreadOp.sleep(bigsleep);
        if(ok) ok=SocketOp.fmt( data->socket, "HTTP/1.1 200 OK\r\n" );
        if(ok) ok=SocketOp.fmt( data->socket, "Content-type: application/xml\r\n\r\n" );
        if(ok) ok=SocketOp.write( data->socket, "<fb id=\"kees\"/>", StrOp.len("<fb id=\"kees\"/>") );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, "/logo.png" ) ) {
        __getImage( inst, ROCWEB_LOGO, True );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, ".png" ) ) {
        char* symbolfile = StrOp.dup( StrOp.find( str, " /" ) + 2 ) ;
        char* p = StrOp.find( symbolfile, "HTTP" );

        if( p != NULL ) {
          p--;
          *p = '\0';
          __getImage( inst, symbolfile, False );
        }
        StrOp.free( symbolfile );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, ".svg" ) ) {
        char* symbolfile = StrOp.dup( StrOp.find( str, " /" ) + 2 ) ;
        char* p = StrOp.find( symbolfile, "HTTP" );

        if( p != NULL ) {
          p--;
          *p = '\0';
          __getSVG( inst, symbolfile );
        }
        StrOp.free( symbolfile );
      }

      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "disconnect... " );
      SocketOp.disConnect( data->socket );
    }
  }
  
  TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "ready work for rocWebME" );

  return True;
}

