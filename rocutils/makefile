# ------------------------------------------------------------
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
ifeq ($(PLATFORM),WIN32)
	LIBS=-liphlpapi -lmpr -lws2_32 -ladvapi32 $(SSLLIBS)
	COREDIR=win
	BINSUFFIX=.exe
	CC_EXTRA_FLAGS=-mthreads
else
	COREDIR=unx
	BINSUFFIX=
	CC_EXTRA_FLAGS=-fPIC
endif
ifeq ($(PLATFORM),WIN64)
    LIBS=-liphlpapi -lmpr -lmswsock -lws2_32 -ladvapi32 $(SSLLIBS)
    COREDIR=win
    BINSUFFIX=.exe
    CC_EXTRA_FLAGS=
endif 

MOUNTPOINT=..
OUTDIR=..$(FS)$(COREDIR)bin
GENDIR=..$(FS)$(COREDIR)gen
TMPOUTDIR=$(GENDIR)$(FS)rocutils$(FS)bin
DEBUG=-g
BINDIR=..$(FS)$(COREDIR)bin

AR=$(TOOLPREFIX)ar

CPP=$(TOOLPREFIX)gcc
LNK=$(TOOLPREFIX)gcc

INCL_PATH=$(MOUNTPOINT)
CC_FLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) -ansi -I$(INCL_PATH) -I$(GENDIR)

OBJS=$(patsubst impl/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/*.c))


all: $(OUTDIR)$(FS)librocutils.a

$(OUTDIR)$(FS)librocutils.a: $(OBJS)
	$(AR) rcs $(OUTDIR)$(FS)librocutils.a $(OBJS)

$(TMPOUTDIR)/%.o: impl/%.c
	$(CPP) $(CC_FLAGS) $< -o $@


clean:
	-$(MKDIR) $(OUTDIR)
	-$(MKDIR) $(TMPOUTDIR)
	-$(MKDIR) $(GENDIR)$(FS)rocutils
	-$(REMOVE) $(TMPOUTDIR)$(FS)*.o
	-$(REMOVE) -f $(OUTDIR)$(FS)librocutils.a

	cd $(GENDIR)$(FS)rocutils$(CS) ..$(FS)..$(FS)rocs$(FS)bin$(FS)ogen$(GENSUFFIX) ..$(FS)..$(FS)rocutils$(FS)rocutils.xml
	
