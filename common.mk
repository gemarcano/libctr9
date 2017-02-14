INCPATHS=-I$(top_srcdir)/include -I$(prefix)/include/freetype2

#THUMBFLAGS=-mthumb #-mthumb-interwork
SIZE_OPTIMIZATION = -flto
#SIZE_OPTIMIZATION = -Wl,--gc-sections -ffunction-sections
AM_CPPFLAGS=$(INCPATHS)
COMMON_FLAGS= -O2 -g -fomit-frame-pointer -ffast-math $(SIZE_OPTIMIZATION) $(C9FLAGS)
ARM_ONLY= -std=gnu11 $(COMMON_FLAGS) $(WARN_CFLAGS) $(SIZE_OPTIMIZATION)
AM_CFLAGS= $(ARM_ONLY) $(THUMBFLAGS)
AM_CXXFLAGS= -std=gnu++14 $(COMMON_FLAGS) $(WARN_CXXFLAGS) $(THUMBFLAGS)
AM_LDFLAGS=-Wl,--use-blx,--pic-veneer,-q $(WARN_LDFLAGS)
OCFLAGS=--set-section-flags .bss=alloc,load,contents

