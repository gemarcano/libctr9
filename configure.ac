AC_INIT([libctr9], [0.0.2], [], [])
AM_INIT_AUTOMAKE([foreign no-dist-gzip dist-xz subdir-objects])

#Find AR and RANLIB for lto support before initializing libtool
AC_CHECK_TOOL([AR],gcc-ar)
AC_CHECK_TOOL([RANLIB],gcc-ranlib)

AC_PROG_CC
AC_PROG_CXX
AM_PROG_AS
AC_CHECK_TOOL([OBJCOPY],objcopy)

AX_CFLAGS_WARN_ALL
AX_COMPILER_FLAGS(,,,[-Wpedantic -Wall -Wextra -Wcast-align -Wcast-qual\
 -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op\
 -Wmissing-declarations -Wmissing-include-dirs -Wredundant-decls\
 -Wshadow -Wsign-conversion -Wstrict-overflow=5 -Wswitch-default\
 -Wundef -Wno-unused -fstrict-aliasing])
AX_CHECK_COMPILE_FLAG(-Wno-declaration-after-statement,
	[WARN_CFLAGS="$WARN_CFLAGS -Wno-declaration-after-statement -Wno-error=declaration-after-statement"], [], [], [])

cflags_save="$CFLAGS"
cxxflags_save="$CXXFLAGS"

THUMBFLAGS="-mthumb"#-mthumb-interwork
C9FLAGS="-mcpu=arm946e-s -mlittle-endian -mword-relocations"
AC_SUBST(C9FLAGS)
AC_SUBST(THUMBFLAGS)
CFLAGS="$C9FLAGS $THUMBFLAGS $CFLAGS"
CXXFLAGS="$C9FLAGS $THUMBFLAGS $CXXFLAGS"
LT_INIT

CFLAGS="$cflags_save"
CXXFLAGS="$cxxflags_save"

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile examples/Makefile libctr9.pc])

AC_OUTPUT

