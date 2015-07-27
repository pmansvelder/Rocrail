/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#ifdef __linux__
#include <sys/resource.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif

#include "rocnetnode/public/rocnetnode.h"

#include "rocrail/wrapper/public/Cmdline.h"

#include "rocs/public/mem.h"
#include "rocs/public/cmdln.h"

static void __signalHandler( int sig ) {
  char* sigName = "";

  switch( sig ) {
  case SIGSEGV:
    sigName = "Segment violation.";
    break;
  case SIGTERM:
    sigName = "Software termination signal from kill.";
    break;
#ifdef SIGKILL
  case SIGKILL:
    sigName = "Software termination.";
    break;
#endif
  case SIGINT:
    sigName = "Interrupt.";
    break;
  case SIGABRT:
    sigName = "Abnormal termination triggered by abort call.";
    break;
  case SIGFPE:
    sigName = "Floating point exception.";
    break;
  case SIGILL:
    sigName = "Illegal instruction - invalid function image.";
    break;
#ifdef SIGPIPE
  case SIGPIPE:
    sigName = "Broken pipe.";
    return;
#endif
  }

  printf( "__signalHandler: %d %s\n", sig, sigName );

  printf( "__signalHandler: shutdown...\n" );
  if( sig != SIGSEGV ) {
    RocNetNodeOp.shutdown();
  }
  else {
    /* try todo a power off... */
    RocNetNodeOp.stop();
    /* Reactivate default handling. */
    printf( "__signalHandler: Reactivate default handling...\n" );
    signal( sig, SIG_DFL );
    printf( "__signalHandler: Raise signal...\n" );
    raise( sig );
  }

  printf( "__signalHandler: exit...\n" );
}


static int m_argc;
static char** m_argv;



static iORocNetNode mRocNetNode = NULL;
int main( int argc, char** argv) {
  /* Resets memory statistics. */
  printf( "Resets memory statistics.\n" );
  MemOp.resetDump();

  /* disable stdout buffering to be able to tail the nohup out*/
  printf( "disable stdout buffering.\n" );
  setbuf( stdout, NULL );


  /* make copy of arguments for later use: */
  printf( "make copy of arguments.\n" );
  m_argc = argc;
  {
    int n = 0;
    m_argv = allocMem( m_argc * sizeof( char* ) );
    for( n = 0; n < m_argc; n++ ) {
      m_argv[n] = StrOp.dup(argv[n]);
    }
  }

  /* Initialize the signal handler. */
  printf( "Initialize the signal handler.\n" );
  /* Not all OS's support this signal. */
#ifdef SIGBREAK
  signal( SIGBREAK, &__signalHandler );   /* Ctrl-Break sequence */
#endif
  signal( SIGTERM , &__signalHandler );   /* Software termination signal from kill */
  signal( SIGINT  , &__signalHandler );   /* interrupt */
  signal( SIGABRT , &__signalHandler );   /* abnormal termination triggered by abort call */
  signal( SIGFPE  , &__signalHandler );   /* floating point exception */
  signal( SIGILL  , &__signalHandler );   /* illegal instruction - invalid function image */
  signal( SIGSEGV , &__signalHandler );   /* segment violation */
#ifdef SIGKILL
  signal( SIGKILL , &__signalHandler );   /* kill */
#endif
#ifdef SIGPIPE
  signal (SIGPIPE, SIG_IGN);
#endif
#ifdef __linux__
  {
    struct rlimit rl;
    getrlimit( RLIMIT_CORE, &rl );
    rl.rlim_cur = 1024 * 1024 * 1024;
    setrlimit( RLIMIT_CORE, &rl );
    printf( "   --                     \n" );
    printf( "  / /  (_)__  __ ____  __ \n" );
    printf( " / /__/ / _ \\/ // /\\ \\/ / \n" );
    printf( "/____/_/_//_/\\_,_/ /_/\\_\\ \n" );
  }
#endif




  printf( "Initialize the Node...\n" );
  mRocNetNode = RocNetNodeOp.inst(NULL);
  return RocNetNodeOp.Main( mRocNetNode, argc, argv );
}
