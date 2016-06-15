include $(top_srcdir)/common.mk

EXTRA_PROGRAMS = test
test_CFLAGS=$(AM_CFLAGS) -T$(srcdir)/arm9loaderhax.ld -nostartfiles -nostdlib -L${CTRARM9}/arm-none-eabi/lib
test_LDFLAGS=$(AM_LDFLAGS) -lctr -lc -lgcc
test_SOURCES = main.c start.s test.c test.h interrupt.c
test_LDADD = $(top_builddir)/src/libctr9.a ${CTRARM9}/arm-none-eabi/lib/thumb/libc.a ${DEVKITARM}/lib/gcc/arm-none-eabi/5.3.0/thumb/libgcc.a 

test-interrupt.${OBJEXT} : AM_CFLAGS += -marm

EXTRA_DIST = arm9loaderhax.ld

FORCE:

$(top_builddir)/src/libctr9.a: FORCE
	(cd $(top_builddir)/src && $(MAKE) $(AM_MAKEFLAGS) libctr9.a )

clean-local:
	rm -f test
	rm -f test.bin

test.bin: test
	$(OBJCOPY) $(OCFLAGS) -O binary test test.bin

