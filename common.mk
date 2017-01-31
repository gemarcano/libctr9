INCPATHS=-I$(top_srcdir)/include -I$(prefix)/include/freetype2

THUMBFLAGS=-mthumb #-mthumb-interwork
SIZE_OPTIMIZATION = -flto
#SIZE_OPTIMIZATION = -Wl,--gc-sections -ffunction-sections
AM_CPPFLAGS=$(INCPATHS)
ARM_ONLY= -std=gnu11 -O2 -g -fomit-frame-pointer -ffast-math \
	-Wpedantic -Wall -Wextra -Wcast-align -Wcast-qual \
	-Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op \
	-Wmissing-declarations -Wmissing-include-dirs -Wredundant-decls \
	-Wshadow -Wsign-conversion -Wstrict-overflow=5 -Wswitch-default \
	-Wundef -Wno-unused $(SIZE_OPTIMIZATION)
AM_CFLAGS=$(ARM_ONLY) $(THUMBFLAGS)
AM_LDFLAGS=-Wl,--use-blx,--pic-veneer,-q
OCFLAGS=--set-section-flags .bss=alloc,load,contents

