/*
 Rocrail - Model Railroad Software
 Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net
 All rights reserved.
*/

#include "rocabox/impl/archivebox_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/file.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"
#include "rocs/public/doc.h"
#include "rocs/public/dir.h"
#include "rocs/public/list.h"

#include "rocabox/wrapper/public/ArchiveBox.h"
#include "rocabox/wrapper/public/Stub.h"

#include <string.h>

static int instCnt = 0;

static void __writeIni(iOArchiveBox inst);

#define StubExt    ".abox"

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOArchiveBoxData data = Data(inst);
    /* Cleanup data->xxx members...*/
    
    freeMem( data );
    freeMem( inst );
    instCnt--;
  }
  return;
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

/** ----- OArchiveBox ----- */
static void _addCategory( obj inst ,const char* category ) {
  iOArchiveBoxData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "add category [%s]", category );
  if( StrOp.findi(wArchiveBox.getcategories(data->ini), category ) == NULL ) {
    char* categories = StrOp.fmt("%s,%s", wArchiveBox.getcategories(data->ini), category );
    wArchiveBox.setcategories(data->ini, categories);
    StrOp.free(categories);
    __writeIni((iOArchiveBox)inst);
  }
  return;
}


static iONode __readStub(const char* stubFilename) {
  iONode stub = NULL;
  if( FileOp.exist(stubFilename) ) {
    iOFile f      = NULL;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "read stub [%s]", stubFilename );
    f = FileOp.inst(stubFilename, OPEN_READONLY);
    if( f != NULL ) {
      iODoc stubDoc = NULL;
      char* xmlStr = (char*)allocMem( FileOp.size( f ) +1 );
      FileOp.read(f, xmlStr, FileOp.size( f ));
      FileOp.base.del(f);

      stubDoc = DocOp.parse( xmlStr );
      if( stubDoc != NULL ) {
        stub = DocOp.getRootNode( stubDoc );
        DocOp.base.del(stubDoc);
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "stub text = [%s]", wStub.gettext(stub) );
      }

      freeMem(xmlStr);
    }
  }
  return stub;
}


static Boolean __findInText(const char* stubText, const char* searchText) {
  Boolean found = True;
  iOStrTok tok = StrTokOp.inst( searchText, '&' );
  while( StrTokOp.hasMoreTokens(tok) ) {
    const char* text = StrTokOp.nextToken( tok );
    if( StrOp.findi(stubText, text) == NULL ) {
      found = False;
      break;
    }
  }
  StrTokOp.base.del(tok);
  return found;
}

static iOList __find( const char* directory ,const char* text, Boolean intext, Boolean incategory, Boolean infilename, Boolean indate, const char* fromdate, const char* todate ) {
  iOList list = ListOp.inst();
  char* filepath = NULL;
  if( FileOp.exist(directory) ) {
    iOList listdir = DirOp.listdir( directory, NULL, -1 );
    if( listdir != NULL ) {
      int listsize = ListOp.size(listdir);
      int i = 0;
      for( i = 0; i < listsize; i++) {
        iDirEntry dir = (iDirEntry)ListOp.get( listdir, i );
        filepath = StrOp.fmt("%s%c%s", directory, SystemOp.getFileSeparator(), dir->name);
        if( (StrOp.len(dir->name) == 1 && StrOp.equals(dir->name, ".")) || (StrOp.len(dir->name) == 2 && StrOp.equals(dir->name, "..")) ) {
          StrOp.free(filepath);
          filepath = NULL;
          continue;
        }
        else if( FileOp.isDirectory( filepath ) ) {
          iOList list2 = __find(filepath, text, intext, incategory, infilename, indate, fromdate, todate);
          if( list2 != NULL ) {
            int listSize = ListOp.size(list2);
            int n = 0;
            for( n = 0; n < listSize; n++ )
              ListOp.add(list, ListOp.get(list2, n));
            ListOp.base.del(list2);
          }
        }
        else {
          if( StrOp.endsWith(dir->name, StubExt) ) {
            iONode stub = NULL;
            char* stubName = StrOp.fmt("%s%c%s", directory, SystemOp.getFileSeparator(), dir->name);
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "getDirEntry: %s", stubName );
            /* evaluate the stub... */
            stub = __readStub(stubName);
            /* add to list... */
            if( stub != NULL ) {
              if( (intext && __findInText(wStub.gettext(stub), text)) ||
                  (infilename && StrOp.findi(wStub.getpath(stub), text)) ||
                  (incategory && StrOp.findi(wStub.getcategory(stub), text)) )
              {
                Boolean isInDate = True;
                if( indate ) {
                  int from = strcmp(fromdate, wStub.getfiledate(stub));
                  int to   = strcmp(todate  , wStub.getfiledate(stub));
                  /*
                     from      file          to        file
                    [20140818][20141021]=-1 [20150818][20141021]=1
                    [20140818][20150112]=-1 [20150818][20150112]=7
                   */
                  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "[%s][%s]=%d [%s][%s]=%d",
                      fromdate, wStub.getfiledate(stub), from, todate , wStub.getfiledate(stub), to );
                  if( from > 0 || to < 0 ) {
                    isInDate = False;
                  }
                }
                if( isInDate ) {
                  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "add stub %s [%s]", wStub.getpath(stub), wStub.gettext(stub) );
                  ListOp.add(list, (obj)stub);
                }
              }
            }
            StrOp.free(stubName);
          }
        }
        StrOp.free(filepath);
        filepath = NULL;
      }
      DirOp.cleandirlist( listdir );
    }
  }
  return list;
}


/**  */
static iOList _find( obj inst ,const char* text, Boolean intext, Boolean incategory, Boolean infilename, Boolean indate, const char* fromdate, const char* todate ) {
  iOArchiveBoxData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "find [%s] in [%s] [%d%d%d%d] [%s]-[%s]", text, data->home, intext, incategory, infilename, indate, fromdate, todate );
  return __find(data->home, text, intext, incategory, infilename, indate, fromdate, todate);
}


/**  */
static const char* _getCategories( obj inst, Boolean* readonly) {
  iOArchiveBoxData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "get categories [%s]", wArchiveBox.getcategories(data->ini) );
  *readonly = data->readonly;
  return wArchiveBox.getcategories(data->ini);
}


/**  */
static iONode _getCategory( obj inst ,const char* category ) {
  iOArchiveBoxData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "get category [%s]", category );
  return NULL;
}


static void __replaceStub(iOArchiveBox inst, const char* stubFilename, iONode stub) {
  iOArchiveBoxData data = Data(inst);
  if( MutexOp.wait( data->stubMux ) ) {
    char* xmlStr = NodeOp.base.toString(stub);
    iOFile f = FileOp.inst(stubFilename, OPEN_WRITE);
    if( f != NULL ) {
      FileOp.write(f, xmlStr, StrOp.len(xmlStr)+1);
      FileOp.base.del(f);
    }
    StrOp.free(xmlStr);

    /* Release the mux. */
    MutexOp.post( data->stubMux );
  }
}


static void __writeStub(iOArchiveBox inst, iONode stub) {
  iOArchiveBoxData data = Data(inst);
  if( MutexOp.wait( data->stubMux ) ) {
    char* stamp = StrOp.createStampNoDots();
    char* stubRoot = StrOp.fmt("%s%c%s", data->home, SystemOp.getFileSeparator(), wStub.getcategory(stub) );
    char* stubFilename = StrOp.fmt("%s%c%s%s", stubRoot, SystemOp.getFileSeparator(), stamp, StubExt );
    char*  xmlStr = NULL;
    iOFile f      = NULL;

    wStub.setuid(stub, stamp);
    wStub.setstubfile(stub, stubFilename);

    StrOp.free(stamp);

    if( !FileOp.exist(stubRoot) ) {
      FileOp.mkdir(stubRoot);
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "write stub [%s]", stubFilename );
    xmlStr = NodeOp.base.toString(stub);
    f = FileOp.inst(stubFilename, OPEN_WRITE);
    if( f != NULL ) {
      FileOp.write(f, xmlStr, StrOp.len(xmlStr));
      FileOp.write(f, "\n", 1);
      FileOp.base.del(f);
    }
    StrOp.free(xmlStr);
    StrOp.free(stubRoot);
    StrOp.free(stubFilename);

    ThreadOp.sleep(10);
    /* Release the mux. */
    MutexOp.post( data->stubMux );
  }
}


/**  */
static char* _linkFile( obj inst ,const char* path, const char* filedate, const char* filetime, long size ,const char* text ,const char* category, Boolean link ) {
  iOArchiveBoxData data = Data(inst);
  char* uid = NULL;
  if( data->readonly ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "ArchiveBox is in readonly mode" );
    return uid;
  }
  iONode stub = NodeOp.inst(wStub.name(), NULL, ELEMENT_NODE);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "link file [%s]", path );
  if( link )
    wStub.setpath(stub, path);
  else {
    /* preserve original file separators */
    char* l_path = StrOp.dup(path);
    wStub.setpath(stub, FileOp.ripPath(l_path));
    StrOp.free(l_path);
  }
  wStub.setfiledate(stub, filedate);
  wStub.setfiletime(stub, filetime);
  wStub.setsize(stub, size);
  wStub.settext(stub, text);
  wStub.setcategory(stub, category);
  wStub.setlink(stub, link);
  ArchiveBoxOp.addCategory(inst, category);
  __writeStub((iOArchiveBox)inst, stub);
  uid = StrOp.dup(wStub.getuid(stub));
  NodeOp.base.del(stub);
  return uid;
}


static Boolean _modifyFile( obj inst, const char* uid, const char* stubfile, const char* text, const char* note, const char* category ) {
  iONode stub = __readStub(stubfile);
  if( stub != NULL ) {
    wStub.settext(stub, text);
    wStub.setnote(stub, note);
    __replaceStub((iOArchiveBox)inst, stubfile, stub);
    return True;
  }
  return False;
}


static Boolean _deleteFile( obj inst ,const char* uid ,const char* stubfile ) {
  iOArchiveBoxData data = Data(inst);
  if( data->readonly ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "ArchiveBox is in readonly mode" );
    return False;
  }
  else {
    iONode stub = __readStub(stubfile);
    if( stub != NULL && !wStub.islink(stub) ) {
      char* rootDir = StrOp.fmt("%s%c%s%c%s", data->home, SystemOp.getFileSeparator(), wStub.getcategory(stub), SystemOp.getFileSeparator(), wStub.getuid(stub));
      char* filepath = StrOp.fmt("%s%c%s", rootDir, SystemOp.getFileSeparator(), wStub.getpath(stub));
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "delete link [%s]", rootDir );
      FileOp.remove(filepath);
      FileOp.rmdir(rootDir);
      StrOp.free(rootDir);
      StrOp.free(filepath);
    }
    NodeOp.base.del(stub);
  }
  return FileOp.remove(stubfile);
}


static void __writeIni(iOArchiveBox inst) {
  iOArchiveBoxData data = Data(inst);
  char* iniFilename = StrOp.fmt("%s%c%s", data->home, SystemOp.getFileSeparator(), "rocabox.ini");
  char*  xmlStr = NULL;
  iOFile f      = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "write ini [%s]", iniFilename );
  xmlStr = NodeOp.base.toString(data->ini);
  f = FileOp.inst(iniFilename, OPEN_WRITE);
  if( f != NULL ) {
    FileOp.write(f, xmlStr, StrOp.len(xmlStr)+1);
    FileOp.base.del(f);
  }
  StrOp.free(xmlStr);
}


static void __readIni(iOArchiveBox inst) {
  iOArchiveBoxData data = Data(inst);
  char* iniFilename = StrOp.fmt("%s%c%s", data->home, SystemOp.getFileSeparator(), "rocabox.ini");
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "read ini [%s]", iniFilename );
  if( !FileOp.exist(iniFilename) ) {
    char*  xmlStr = NULL;
    iOFile f      = NULL;
    data->ini = NodeOp.inst(wArchiveBox.name(), NULL, ELEMENT_NODE);
    __writeIni(inst);
  }
  else {
    iOFile f = FileOp.inst(iniFilename, OPEN_READONLY);
    if( f != NULL ) {
      iODoc iniDoc = NULL;
      char* xmlStr = (char*)allocMem( FileOp.size( f ) +1 );
      FileOp.read(f, xmlStr, FileOp.size( f ));
      FileOp.base.del(f);
      iniDoc = DocOp.parse( xmlStr );
      if( iniDoc != NULL ) {
        data->ini = DocOp.getRootNode( iniDoc );
        DocOp.base.del(iniDoc);
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "categories = [%s]", wArchiveBox.getcategories(data->ini) );
      }
      freeMem(xmlStr);
    }
  }

  if( data->ini == NULL ) {
    data->ini = NodeOp.inst(wArchiveBox.name(), NULL, ELEMENT_NODE);
    wArchiveBox.setcategories(data->ini, "Genereal");
  }

  StrOp.free(iniFilename);
}

#define DATABLOCK 32*1024

static long _getFileData( obj inst, const char* uid, const char* category, const char* filename, int nr, byte* bytes, Boolean* lastBlock ) {
  iOArchiveBoxData data = Data(inst);
  long bufferSize = 0;
  char* rootDir = StrOp.fmt("%s%c%s%c%s", data->home, SystemOp.getFileSeparator(), category, SystemOp.getFileSeparator(), uid);
  char* filepath = StrOp.fmt("%s%c%s", rootDir, SystemOp.getFileSeparator(), filename);

  iOFile f = FileOp.inst(filepath, OPEN_READONLY);
  if( f != NULL ) {
    long totalsize = FileOp.fileSize(filepath);

    bufferSize = DATABLOCK;
    if( (totalsize - nr*DATABLOCK <  DATABLOCK) || (totalsize - nr*DATABLOCK == DATABLOCK) ) {
      bufferSize = totalsize - nr*DATABLOCK;
      *lastBlock = True;
    }
    else
      *lastBlock = False;

    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999,
        "get file data: uid=%s size=%ld nr=%d totalsize=%ld (last=%s)",
        uid, bufferSize, nr, totalsize, *lastBlock?"true":"false" );

    FileOp.setpos(f, nr*DATABLOCK);
    FileOp.read(f, (char*)bytes, bufferSize);
    FileOp.base.del(f);
  }

  StrOp.free(rootDir);
  StrOp.free(filepath);

  return bufferSize;
}


static Boolean _fileData( obj inst, const char* uid, const char* category, const char* filename, long size, int nr, long totalsize, const byte* bytes ) {
  iOArchiveBoxData data = Data(inst);
  if( data->readonly ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "ArchiveBox is in readonly mode" );
    return False;
  }
  else {
    char* rootDir = StrOp.fmt("%s%c%s%c%s", data->home, SystemOp.getFileSeparator(), category, SystemOp.getFileSeparator(), uid);
    char* filepath = StrOp.fmt("%s%c%s", rootDir, SystemOp.getFileSeparator(), filename);
    iOFile f = NULL;

    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "file data: uid=%s size=%ld nr=%d totalsize=%ld", uid, size, nr, totalsize );
    if( !FileOp.exist(rootDir) )
      FileOp.mkdir(rootDir);

    f = FileOp.inst(filepath, OPEN_APPEND);
    if( f != NULL ) {
      FileOp.write(f, (char*)bytes, size);
      FileOp.base.del(f);
    }

    StrOp.free(rootDir);
    StrOp.free(filepath);
  }
  return True;
}

/**  */
static struct OArchiveBox* _inst( const char* home ,const iOTrace trc, int readonly ) {
  iOArchiveBox __ArchiveBox = allocMem( sizeof( struct OArchiveBox ) );
  iOArchiveBoxData data = allocMem( sizeof( struct OArchiveBoxData ) );
  MemOp.basecpy( __ArchiveBox, &ArchiveBoxOp, 0, sizeof( struct OArchiveBox ), data );

  TraceOp.set( trc );
  /* Initialize data->xxx members... */
  data->home     = home;
  data->readonly = readonly;
  data->stubMux  = MutexOp.inst( NULL, True );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "------------------------------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ArchiveBox" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  home     = [%s]", data->home );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  readonly = [%s]", data->readonly?"true":"false" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  All rights reserved." );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "------------------------------------------------------------" );

  if( !data->readonly && !FileOp.exist(data->home) ) {
    FileOp.mkdir(data->home);
  }

  __readIni(__ArchiveBox);

  instCnt++;
  return __ArchiveBox;
}

/* Support for dynamic Loading */
iOArchiveBox getArchiveBox( const char* home ,const iOTrace trc, int readonly )
{
  return _inst(home,trc, readonly);
}



/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocabox/impl/archivebox.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
