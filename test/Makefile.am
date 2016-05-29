include $(top_srcdir)/common.mk

EXTRA_PROGRAMS = test
test_CFLAGS=$(AM_CFLAGS) -T$(srcdir)/arm9loaderhax.ld -nostartfiles
test_LDFLAGS=$(AM_LDFLAGS) -lctr
test_SOURCES = main.c start.s test.c test.h
test_LDADD = $(top_builddir)/src/libctr9.a

EXTRA_DIST = arm9loaderhax.ld

FORCE:

$(top_builddir)/src/libctr9.a: FORCE
	(cd $(top_builddir)/src && $(MAKE) $(AM_MAKEFLAGS) libctr9.a )

clean-local:
	rm -f test
	rm -f test.bin

test.bin: test
	$(OBJCOPY) $(OCFLAGS) -O binary test test.bin

