include $(top_srcdir)/common.mk

bin_PROGRAMS = test
test_CFLAGS=$(AM_CFLAGS) --specs=arm9loaderhax.specs -nostartfiles
test_LDFLAGS=$(AM_LDFLAGS) -lctr
test_SOURCES = main.c start.s
test_LDADD = $(top_builddir)/src/libctr9.a

all-local: test.bin
clean-local:
	rm -f test.bin

test.bin: test
	$(OBJCOPY) $(OCFLAGS) -O binary test test.bin

