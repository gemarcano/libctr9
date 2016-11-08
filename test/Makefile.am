include $(top_srcdir)/common.mk

EXTRA_PROGRAMS = test
test_CFLAGS=$(AM_CFLAGS) -v  -Wl,--verbose -T$(srcdir)/arm9loaderhax.ld -L${prefix}/lib -I${prefix}/include/freetype2 #-nostartfiles
test_SOURCES = main.c start.s test.c test.h interrupt.c nand_tests.h nand_tests.c nand_crypto_tests.h nand_crypto_tests.c sd_tests.h sd_tests.c twl_tests.c twl_tests.h memory_tests.h memory_tests.c crypto_memory_tests.h crypto_memory_tests.c memory_control_tests.c memory_control_tests.h
test_LDADD = $(AM_LDADD) $(top_builddir)/src/libctr9.a -lfreetype -lctr -lm

EXTRA_DIST = arm9loaderhax.ld

.PHONY: FORCE

FORCE:

$(top_builddir)/src/libctr9.a: FORCE
	(cd $(top_builddir)/src && $(MAKE) $(AM_MAKEFLAGS) libctr9.a )

clean-local:
	rm -f test
	rm -f test.bin

test.bin: test
	cp test test.bin
	arm-none-eabi-strip test.bin

