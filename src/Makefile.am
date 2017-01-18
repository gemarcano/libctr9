include $(top_srcdir)/common.mk

noinst_LTLIBRARIES = libctr9_arm.la
libctr9_arm_la_SOURCES = ctr_interrupt.c ctr_irq.c
libctr9_arm_la_CFLAGS = $(ARM_ONLY) -marm

lib_LTLIBRARIES = libctr9.la
libctr9_la_CCASFLAGS = -I$(top_srcdir)/data
libctr9_la_SOURCES = aes.c ctr_nand_crypto_interface.c ctr_sd_interface.c sha.c \
	ctr_io_interface.c ctr_nand_interface.c sdmmc/delay.s sdmmc/sdmmc.c i2c.c \
	ctr_rtc.c fatfs/diskio.c fatfs/ff.c fatfs/option/unicode.c ctr_system.c \
	fatfs/ctr_fatfs_disk.c ctr_disks.c ctr_fatfs.c \
	ctr_interrupt_asm.s ctr_screen.c ctr_hid.c ctr_pxi.c gamecart/command_ctr.c \
	gamecart/command_ntr.c  gamecart/protocol.c  gamecart/protocol_ctr.c  \
	gamecart/protocol_ntr.c ctr_cart_interface.c gamecart/delay.s ctr_headers.c \
	ctr_timer.c ctr_system_clock.c ctr_cache.s \
	ctr_crypto_interface.c ctr_memory_interface.c ctr_memory.c ctr_memory_asm.s \
	ctr_aeskeydb.c ctr_firm.c ctr_drives.c ctr_console.c \
	ctr_file_interface.c ctr_freetype.c crt0.s ctr_init.c ctr_circular_buffer.c\
	ctr_elf_loader.c
libctr9_la_LIBADD = libctr9_arm.la -lctr_core -lctrelf -lfreetype
libctr9_la_LDFLAGS = $(AM_LDFLAGS) -static

libctr9dir = $(pkgdatadir)
libctr9_DATA=linker.ld

EXTRA_libctr9_la_SOURCES = fatfs/option/cc932.c fatfs/option/cc936.c \
	fatfs/option/cc949.c fatfs/option/cc950.c fatfs/option/ccsbcs.c \
	fatfs/option/syscall.c fatfs/option/unicode.c

includedirectory = $(top_srcdir)/include

ctrincludedir = $(includedir)/ctr9
ctrioincludedir = $(includedir)/ctr9/io
ctrgamecartincludedir = $(includedir)/ctr9/gamecart
ctrfatfsincludedir = $(includedir)/ctr9/io/fatfs
ctrsdmmcincludedir = $(includedir)/ctr9/io/sdmmc

#do not install fatfs headers
noinst_HEADERS = $(includedirectory)/ctr9/io/fatfs/ff.h \
	$(includedirectory)/ctr9/io/fatfs/ctr_fatfs_disk.h \
	$(includedirectory)/ctr9/io/fatfs/diskio.h \
	$(includedirectory)/ctr9/io/fatfs/ffconf.h \
	$(includedirectory)/ctr9/io/fatfs/ctr_fatfs.h \
	$(includedirectory)/ctr9/io/fatfs/integer.h

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
	$(includedirectory)/ctr9/ctr_cache.h \
	$(includedirectory)/ctr9/ctr_memory.h \
	$(includedirectory)/ctr9/ctr_aeskeydb.h \
	$(includedirectory)/ctr9/ctr_firm.h \
	$(includedirectory)/ctr9/ctr_freetype.h \
	$(includedirectory)/ctr9/ctr_circular_buffer.h \
	$(includedirectory)/ctr9/ctr_elf_loader.h

ctrgamecartinclude_HEADERS = $(includedirectory)/ctr9/gamecart/protocol.h \
	$(includedirectory)/ctr9/gamecart/command_ctr.h \
	$(includedirectory)/ctr9/gamecart/command_ntr.h \
	$(includedirectory)/ctr9/gamecart/protocol_ctr.h \
	$(includedirectory)/ctr9/gamecart/protocol_ntr.h

ctrioinclude_HEADERS = \
	$(includedirectory)/ctr9/io/ctr_io_interface.h \
	$(includedirectory)/ctr9/io/ctr_nand_crypto_interface.h \
	$(includedirectory)/ctr9/io/ctr_nand_interface.h \
	$(includedirectory)/ctr9/io/ctr_sd_interface.h \
	$(includedirectory)/ctr9/io/ctr_sdmmc_implementation.h \
	$(includedirectory)/ctr9/io/ctr_sdmmc_implementation.c \
	$(includedirectory)/ctr9/io/ctr_disks.h \
	$(includedirectory)/ctr9/io/ctr_cart_interface.h \
	$(includedirectory)/ctr9/io/ctr_io_implementation.h \
	$(includedirectory)/ctr9/io/ctr_io_implementation.c \
	$(includedirectory)/ctr9/io/ctr_crypto_interface.h \
	$(includedirectory)/ctr9/io/ctr_memory_interface.h \
	$(includedirectory)/ctr9/io/ctr_drives.h \
	$(includedirectory)/ctr9/io/ctr_console.h \
	$(includedirectory)/ctr9/io/ctr_file_interface.h

ctrsdmmcinclude_HEADERS = $(includedirectory)/ctr9/io/sdmmc/sdmmc.h

