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


#include "rocs/public/str.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/socket.h"
#include "rocs/public/file.h"



static const char* ROCWEB_INDEX = "web/index.html";
static const char* ROCWEB_JS    = "web/rocweb.js";
static const char* ROCWEB_CSS   = "web/rocweb.css";

static void __getFile(iOPClient inst, const char* fname) {
  iOPClientData data = Data(inst);

  if( FileOp.exist( fname ) ) {
    long size = FileOp.fileSize( fname );
    char* html = allocMem( size + 1 );
    iOFile f = FileOp.inst( fname, OPEN_READONLY );
    if( f != NULL ) {
      Boolean ok = True;
      FileOp.read( f, html, size );
      FileOp.base.del( f );
      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "write %s %d", fname, size );
      if(ok) ok=SocketOp.fmt( data->socket, "HTTP/1.0 200 OK\r\n" );
      if(ok) ok=SocketOp.fmt( data->socket, "Content-type: text/html\r\n\r\n" );
      if(ok) ok=SocketOp.write( data->socket, (char*)html, size );
    }
    freeMem(html);
  }
}

static void __getImage(iOPClient inst, const char* fname) {
  iOPClientData data = Data(inst);

  if( FileOp.exist( fname ) ) {
    long size = FileOp.fileSize( fname );
    char* html = allocMem( size + 1 );
    iOFile f = FileOp.inst( fname, OPEN_READONLY );
    if( f != NULL ) {
      Boolean ok = True;
      FileOp.read( f, html, size );
      FileOp.base.del( f );
      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "write %s %d", fname, size );
      if(ok) ok=SocketOp.fmt( data->socket, "HTTP/1.0 200 OK\r\n" );
      if(ok) ok=SocketOp.fmt( data->socket, "Content-type: image/%s\r\n\r\n", "png" );
      if(ok) ok=SocketOp.write( data->socket, (char*)html, size );
    }
    freeMem(html);
  }
}

static void __getModel(iOPClient inst) {
  iOPClientData data = Data(inst);
  iONode model = ModelOp.getModel( AppOp.getModel() );
  char* xml = NodeOp.base.toString( model );
  Boolean ok = True;
  int size = StrOp.len(xml);
  TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "write model %d", size );
  if(ok) ok=SocketOp.fmt( data->socket, "HTTP/1.0 0 OK\r\n" );
  if(ok) ok=SocketOp.fmt( data->socket, "Content-type: application/xml\r\n\r\n" );
  if(ok) ok=SocketOp.write( data->socket, xml, size );
  StrOp.free(xml);
}


Boolean rocWebME( iOPClient inst, const char* str ) {
  TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "work for rocWebME [%s]", str );
  
  if( inst != NULL ) {
    iOPClientData data = Data(inst);
    char l_str[1025] = {'\0'};

    if( StrOp.find( str, "GET" ) && StrOp.find( str, " / " ) ) {
      __getFile( inst, ROCWEB_INDEX );
    }
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, "/rocweb.js" ) ) {
      __getFile( inst, ROCWEB_JS );
    }
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, "/rocweb.css" ) ) {
      __getFile( inst, ROCWEB_CSS );
    }
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, "/plan.xml" ) ) {
      Boolean ok = True;
      __getModel( inst );
    }
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, "/rocweb.xml?" ) ) {
      Boolean ok = True;
      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "command: %s", str );
      if(ok) ok=SocketOp.fmt( data->socket, "HTTP/1.0 202 OK\r\n" );
      if(ok) ok=SocketOp.fmt( data->socket, "Content-type: application/xml\r\n\r\n" );
    }
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, "/update.xml" ) ) {
      Boolean ok = True;
      int randNumber = rand();
      int bigsleep = randNumber % 10000;

      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "update...sleep=%d", bigsleep );
      /* ToDo: get an update event from queue. */
      ThreadOp.sleep(bigsleep);
      if(ok) ok=SocketOp.fmt( data->socket, "HTTP/1.0 200 OK\r\n" );
      if(ok) ok=SocketOp.fmt( data->socket, "Content-type: application/xml\r\n\r\n" );
      if(ok) ok=SocketOp.write( data->socket, "<fb id=\"kees\"/>", StrOp.len("<fb id=\"kees\"/>") );
    }
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, ".png" ) ) {
      char* symbolfile = StrOp.dup( StrOp.find( str, " /" ) + 2 ) ;
      char* p = StrOp.find( symbolfile, "HTTP" );

      if( p != NULL ) {
        p--;
        *p = '\0';
        __getImage( inst, symbolfile );
      }
      StrOp.free( symbolfile );
    }

    TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "Reading rest of HTTP header... " );
    while( SocketOp.readln( data->socket, l_str ) && !SocketOp.isBroken( data->socket ) ) {
      if( l_str[0] == '\r' || l_str[0] == '\n' ) {
        break;
      }
      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "%s", l_str );
    };
    
    TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "disconnect... " );
    SocketOp.disConnect( data->socket );
  }
  
  return True;
}

