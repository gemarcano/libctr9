include $(top_srcdir)/common.mk

bin_PROGRAMS = test
test_CFLAGS=$(AM_CFLAGS) --specs=arm9loaderhax.specs -nostartfiles
test_LDFLAGS=$(AM_LDFLAGS) -lctr
test_SOURCES = main.c start.s
