include $(top_srcdir)/common.mk

lib_LIBRARIES = libctr9.a
libctr9_a_SOURCES = aes.c ctr_nand_crypto_interface.c ctr_sd_interface.c sha.c \
	ctr_io_interface.c ctr_nand_interface.c sdmmc/delay.s sdmmc/sdmmc.c i2c.c \
	ctr_rtc.c fatfs/diskio.c fatfs/ff.c fatfs/option/unicode.c ctr_system.c \
	ctr_fatfs_interface.c fatfs/ctr_fatfs_disk.c ctr_disks.c \
	fatfs/ctr_fatfs_disk.c ctr_fatfs.c ctr_interrupt.c \
	ctr_interrupt_asm.s ctr_screen.c ctr_hid.c ctr_pxi.c gamecart/command_ctr.c \
	gamecart/command_ntr.c  gamecart/protocol.c  gamecart/protocol_ctr.c  \
	gamecart/protocol_ntr.c ctr_cart_interface.c gamecart/delay.s ctr_headers.c \
	ctr_timer.c ctr_irq.c ctr_system_clock.c ctr_gfx.c ctr_cache.s \
	ctr_crypto_interface.c

ctr_interrupt.o : AM_CFLAGS += -marm #Force ARM mode for ctr_interrupt
ctr_irq.o : AM_CFLAGS += -marm #Force ARM mode for ctr_system_ARM

EXTRA_libctr9_a_SOURCES = fatfs/option/cc932.c fatfs/option/cc936.c \
	fatfs/option/cc949.c fatfs/option/cc950.c fatfs/option/ccsbcs.c \
	fatfs/option/syscall.c fatfs/option/unicode.c

includedirectory = $(top_srcdir)/include

ctrincludedir = $(includedir)/ctr9
ctrioincludedir = $(includedir)/ctr9/io
ctrgamecartincludedir = $(includedir)/ctr9/gamecart
ctrfatfsincludedir = $(includedir)/ctr9/io/fatfs
ctrsdmmcincludedir = $(includedir)/ctr9/io/sdmmc

ctrinclude_HEADERS = $(includedirectory)/ctr9/aes.h \
	$(includedirectory)/ctr9/ctr_rtc.h $(includedirectory)/ctr9/ctr_system.h \
	$(includedirectory)/ctr9/i2c.h $(includedirectory)/ctr9/io.h \
	$(includedirectory)/ctr9/sha.h $(includedirectory)/ctr9/ctr_interrupt.h \
	$(includedirectory)/ctr9/ctr_screen.h \
	$(includedirectory)/ctr9/ctr_hid.h \
	$(includedirectory)/ctr9/ctr_pxi.h \
	$(includedirectory)/ctr9/ctr_headers.h \
	$(includedirectory)/ctr9/ctr_timer.h \
	$(includedirectory)/ctr9/ctr_irq.h \
	$(includedirectory)/ctr9/ctr_system_clock.h \
	$(includedirectory)/ctr9/ctr_gfx.h \
	$(includedirectory)/ctr9/ctr_cache.h

ctrgamecartinclude_HEADERS = $(includedirectory)/ctr9/gamecart/protocol.h \
	$(includedirectory)/ctr9/gamecart/command_ctr.h \
	$(includedirectory)/ctr9/gamecart/command_ntr.h \
	$(includedirectory)/ctr9/gamecart/protocol_ctr.h \
	$(includedirectory)/ctr9/gamecart/protocol_ntr.h

ctrioinclude_HEADERS = $(includedirectory)/ctr9/io/ctr_fatfs_interface.h \
	$(includedirectory)/ctr9/io/ctr_io_interface.h \
	$(includedirectory)/ctr9/io/ctr_nand_crypto_interface.h \
	$(includedirectory)/ctr9/io/ctr_nand_interface.h \
	$(includedirectory)/ctr9/io/ctr_sd_interface.h \
	$(includedirectory)/ctr9/io/ctr_sdmmc_implementation.h \
	$(includedirectory)/ctr9/io/ctr_sdmmc_implementation.c \
	$(includedirectory)/ctr9/io/ctr_disks.h \
	$(includedirectory)/ctr9/io/ctr_fatfs.h \
	$(includedirectory)/ctr9/io/ctr_cart_interface.h \
	$(includedirectory)/ctr9/io/ctr_io_implementation.h \
	$(includedirectory)/ctr9/io/ctr_io_implementation.c \
	$(includedirectory)/ctr9/io/ctr_crypto_interface.h


ctrfatfsinclude_HEADERS = $(includedirectory)/ctr9/io/fatfs/ff.h \
	$(includedirectory)/ctr9/io/fatfs/ctr_fatfs_disk.h \
	$(includedirectory)/ctr9/io/fatfs/diskio.h \
	$(includedirectory)/ctr9/io/fatfs/ffconf.h \
	$(includedirectory)/ctr9/io/fatfs/integer.h

ctrsdmmcinclude_HEADERS = $(includedirectory)/ctr9/io/sdmmc/sdmmc.h

