include $(top_srcdir)/common.mk

EXTRA_PROGRAMS = example1 example2 example3 example4

example1_CFLAGS=$(AM_CFLAGS) -T$(top_srcdir)/src/linker.ld -L${prefix}/lib -I${prefix}/include/freetype2
example1_LDADD = $(AM_LDADD) $(top_builddir)/src/libctr9.a -lfreetype -lctr -lm
example1_SOURCES = example1.c
example2_CFLAGS=$(AM_CFLAGS) -T$(top_srcdir)/src/linker.ld -L${prefix}/lib -I${prefix}/include/freetype2
example2_LDADD = $(AM_LDADD) $(top_builddir)/src/libctr9.a -lfreetype -lctr -lm
example2_SOURCES = example2.c
example3_CFLAGS=$(AM_CFLAGS) -T$(top_srcdir)/src/linker.ld -L${prefix}/lib -I${prefix}/include/freetype2
example3_LDADD = $(AM_LDADD) $(top_builddir)/src/libctr9.a -lfreetype -lctr -lm
example3_SOURCES = example3.c

.PHONY: FORCE

FORCE:

$(top_builddir)/src/libctr9.a: FORCE
	(cd $(top_builddir)/src && $(MAKE) $(AM_MAKEFLAGS) libctr9.a )

clean-local:
	-rm -f example1 example2 example3
