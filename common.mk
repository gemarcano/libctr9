INCPATHS=-I$(top_srcdir)/include
C9FLAGS=-mcpu=arm946e-s -march=armv5te -mlittle-endian

#THUMBFLAGS=-mthumb -mthumb-interwork
AM_CFLAGS= -flto -std=c11 -Os -g -mword-relocations -fomit-frame-pointer -ffast-math \
	-Wpedantic -Wall -Wextra -Wcast-align -Wcast-qual \
	-Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op \
	-Wmissing-declarations -Wmissing-include-dirs -Wredundant-decls \
	-Wshadow -Wsign-conversion -Wstrict-overflow=5 -Wswitch-default \
	-Wundef -Wno-unused $(INCPATHS) $(C9FLAGS)

#OCFLAGS=--set-section-flags .bss=alloc,load,contents

