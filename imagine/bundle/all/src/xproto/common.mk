ifndef CHOST
CHOST := $(shell cc -dumpmachine)
else
buildArg := --build=$(shell cc -dumpmachine)
endif

xprotoVer := 7.0.31
xprotoSrcDir := xproto-$(xprotoVer)
xprotoSrcArchive := xproto-$(xprotoVer).tar.bz2

makeFile := $(buildDir)/Makefile
outputPCFile := $(buildDir)/xproto.pc
installIncludeDir := $(installDir)/include/X11

all : $(outputPCFile)

install : $(outputPCFile)
	@echo "Installing xproto to: $(installDir)"
	@mkdir -p $(installIncludeDir) $(installDir)/lib/pkgconfig
	cp $(outputPCFile) $(installDir)/lib/pkgconfig/
	cp $(xprotoSrcDir)/*.h $(buildDir)/*.h $(installIncludeDir)/

.PHONY : all install

$(xprotoSrcDir)/configure : | $(xprotoSrcArchive)
	@echo "Extracting xproto..."
	tar -mxjf $|
	autoreconf -vfi $(xprotoSrcDir)

$(outputPCFile) : $(makeFile)
	@echo "Building xproto..."
	$(MAKE) -C $(<D)

$(makeFile) : $(xprotoSrcDir)/configure
	@echo "Configuring xproto..."
	@mkdir -p $(@D)
	dir=`pwd` && cd $(@D) && CC="$(CC)" CFLAGS="$(CPPFLAGS) $(CFLAGS)" LD="$(LD)" LDFLAGS="$(LDLIBS)" $$dir/$(xprotoSrcDir)/configure --prefix=$(installDir) --host=$(CHOST) PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) PKG_CONFIG=pkg-config $(buildArg)

