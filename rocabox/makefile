# Rocrail - Model Railroad Software
# Copyright (C) 2002-2015 Robert Jan Versluis, Rocrail.net
# All rights reserved.
# ------------------------------------------------------------
# commandline:
#   make DEBUG=... MOUNTPOINT=... OUTDIR=...
#
# commandline for mingw cross compiling:
#   make PLATFORM=WIN32 clean
#   make TOOLPREFIX=i386-mingw32- PLATFORM=WIN32 all
#
# commandline for mingw native compiling:
#   make PLATFORM=WIN32 NATIVE=WIN32 clean
#   make TOOLPREFIX=i386-mingw32- PLATFORM=WIN32 NATIVE=WIN32 all
#
# commandline for osx compiling:
#   make PLATFORM=MACOSX clean all
#
FS=/
CS=;
COPY=cp
REMOVE=rm -f
GENSUFFIX=
MKDIR=mkdir -p

ifeq ($(NATIVE),WIN32)
	FS=$\\
	CS=&
	COPY=copy
	REMOVE=del /Q
	GENSUFFIX=.exe
	MKDIR=mkdir
endif


PLATFORM=LINUX
ARCH=
ifeq ($(PLATFORM),WIN32)
	LIBS=-liphlpapi -lmpr -lws2_32 -ladvapi32 -lwinmm -lm
	DIRPREFIX=win
	CC_EXTRA_FLAGS=-mthreads
	LNK_FLAGS=--def rocabox.def -mthreads -static
	LNK=$(TOOLPREFIX)dllwrap
	SHAREDSUFFIX=.dll
else ifeq ($(PLATFORM),WIN64)
	DIRPREFIX=win
	LIBS=-liphlpapi -lmpr -lmswsock -lws2_32 -ladvapi32 -lwinmm -lm
	LNK=$(TOOLPREFIX)dllwrap
	LNK_FLAGS=--def rocabox.def
	SHAREDSUFFIX=.dll
	CC_EXTRA_FLAGS=
else ifeq ($(PLATFORM),MACOSX)
	LIBS=-lpthread -ldl -lm
	LIBUSB=-lusb-1.0
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	LNK_FLAGS=-dynamiclib -flat_namespace -undefined suppress -L/usr/local/lib
	LNK=gcc
	SHAREDSUFFIX=.dylib
else ifeq ($(PLATFORM),OPENBSD)
	LIBS=-pthread
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	LNK_FLAGS=-shared
	LNK=gcc
	SHAREDSUFFIX=.so
else
	LIBS=-lpthread -ldl -lm
	LIBUSB=-lusb-1.0
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	LNK_FLAGS=-shared
	LNK=gcc
	SHAREDSUFFIX=.so
endif

MOUNTPOINT=..
OUTDIR=..$(FS)$(DIRPREFIX)bin
GENDIR=..$(FS)$(DIRPREFIX)gen
TMPOUTDIR=$(GENDIR)$(FS)rocabox$(FS)bin
DEBUG=-g
DESTDIR=$(FS)opt$(FS)rocrail
BINDIR=..$(FS)rocs$(FS)bin

CPP=$(TOOLPREFIX)gcc
INCL_PATH=$(MOUNTPOINT)
CC_FLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) -I$(INCL_PATH) -I$(GENDIR)
DDL_CC_FLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) -O2 -I$(INCL_PATH) -I$(GENDIR)
RRLIBS=$(OUTDIR)$(FS)libwrapper.a $(OUTDIR)$(FS)librocutils.a $(OUTDIR)$(FS)librocs.a

OBJS=$(patsubst impl/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/*.c))

all: $(OBJS) $(OUTDIR)$(FS)rocabox$(SHAREDSUFFIX)

# ------------------------------------------------------------
# The Digital Interfaces as shared libraries.
# ------------------------------------------------------------
$(OUTDIR)$(FS)rocabox$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)archivebox.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)rocabox$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)archivebox.o $(COMMONOBJS) $(RRLIBS) $(LIBS)

# ------------------------------------------------------------
# The rocabox objects.
# ------------------------------------------------------------
$(TMPOUTDIR)/%.o: impl/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

clean:
	-$(MKDIR) $(OUTDIR)
	-$(MKDIR) $(TMPOUTDIR)
	-$(MKDIR) $(GENDIR)$(FS)rocabox
	-$(REMOVE) $(TMPOUTDIR)$(FS)*.o
	cd $(GENDIR)$(FS)rocabox$(CS) ..$(FS)$(BINDIR)$(FS)ogen$(GENSUFFIX) ..$(FS)..$(FS)rocabox$(FS)rocabox.xml ..$(FS)

install:
	-cp -p $(OUTDIR)$(FS)rocabox$(SHAREDSUFFIX) $(DESTDIR)
	
