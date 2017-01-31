AC_INIT([libctr9], [0.0.2], [], [])
AM_INIT_AUTOMAKE([foreign no-dist-gzip dist-xz subdir-objects])

#Find AR and RANLIB for lto support before initializing libtool
AC_CHECK_TOOL([AR],gcc-ar)
AC_CHECK_TOOL([RANLIB],gcc-ranlib)

AC_PROG_CC
AC_PROG_CXX
AM_PROG_AS
AC_CHECK_TOOL([OBJCOPY],objcopy)

AC_CHECK_HEADERS([ft2build.h freetype2/ft2build.h], [break], [FT_FOUND=1])

if test "$FT_FOUND" == "1"
then
AC_MSG_ERROR(
[unable to find ft2build.h, make sure FreeType2 is installed and\
 that the right -I flag is set.])
fi

C9FLAGS="-mcpu=arm946e-s -mlittle-endian -mword-relocations"
THUMBFLAGS="-mthumb"#-mthumb-interwork
CFLAGS="$C9FLAGS $CFLAGS"
CXXFLAGS="$C9FLAGS $CXXFLAGS"

cflags_save="$CFLAGS"
cxxflags_save="$CXXFLAGS"

CFLAGS="$THUMBFLAGS $CFLAGS"
CXXFLAGS="$THUMBFLAGS $CXXFLAGS"
LT_INIT
CFLAGS="$cflags_save"
CXXFLAGS="$cxxflags_save"

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile examples/Makefile libctr9.pc])
AC_OUTPUT

