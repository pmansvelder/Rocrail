#    Copyright (C) 2002-2015 Robert Jan Versluis, Rocrail.net
#    All rights reserved.


# commandline for mingw cross compiling:
#   make clean
#   make TOOLPREFIX=i586-mingw32msvc- LIBSUFFIX=-i586-mingw32msvc PLATFORM=WIN32 MINGWINSTALL=/usr/i586-mingw32msvc all
#
# commandline for osx compiling:
#   make PLATFORM=MACOSX
#
FS=/
CS=;
COPY=cp
REMOVE=rm -f
GENSUFFIX=
QUOT="
MAKE ?= make
WXCONFIG=wx-config

ifeq ($(NATIVE),WIN32)
	FS=$\\
	CS=&
	COPY=copy
	REMOVE=del /Q
	GENSUFFIX=.exe
	QUOT=
endif

PLATFORM=LINUX
ARCH=

all: version
	cd rocrail$(CS) $(MAKE) rocrail TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) ARCH=$(ARCH) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL) WXCONFIG=$(WXCONFIG)

server: version
	cd rocrail$(CS) $(MAKE) server TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) ARCH=$(ARCH) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL) WXCONFIG=$(WXCONFIG)

offline: versionoffline
	cd rocrail$(CS) $(MAKE) fromtar TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) ARCH=$(ARCH) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL) WXCONFIG=$(WXCONFIG)

fromtar: version
	cd rocrail$(CS) $(MAKE) fromtar TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) ARCH=$(ARCH) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL) WXCONFIG=$(WXCONFIG)

release: version
	cd rocrail$(CS) $(MAKE) rocrail TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) ARCH=$(ARCH) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL) WXCONFIG=$(WXCONFIG) DEBUG= 
	
install:
	cd rocrail$(CS) $(MAKE) install_all

uninstall:
	cd rocrail$(CS) $(MAKE) uninstall_all


version:
	@echo $(QUOT)const int revisionnr = $(QUOT) > common$(FS)version.h
	git rev-list --count HEAD >> common$(FS)version.h
	@echo $(QUOT);$(QUOT) >> common$(FS)version.h
	@echo $(QUOT)const char* commithash = $(QUOT) >> common$(FS)version.h
	git log -n 1 --pretty=format:\"%H\" >> common$(FS)version.h
	@echo $(QUOT);$(QUOT) >> common$(FS)version.h

versionoffline:
	@echo $(QUOT)const int revisionnr = 0$(QUOT) > common$(FS)version.h
	@echo $(QUOT);$(QUOT) >> common$(FS)version.h
	@echo $(QUOT)const char* commithash = 0$(QUOT) >> common$(FS)version.h
	@echo $(QUOT);$(QUOT) >> common$(FS)version.h

