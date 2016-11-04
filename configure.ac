AC_INIT([libctr9], [0.0.2], [], [])
AM_INIT_AUTOMAKE([foreign no-dist-gzip dist-xz subdir-objects])

#Find AR and RANLIB for lto support before initializing libtool
AC_CHECK_TOOL([AR],gcc-ar)
AC_CHECK_TOOL([RANLIB],gcc-ranlib)
LT_INIT

AC_PROG_CC
AM_PROG_AS
AC_CHECK_TOOL([OBJCOPY],objcopy)

AC_CHECK_HEADERS([ft2build.h freetype2/ft2build.h], [break], [FT_FOUND=1])

if test $FT_FOUND == 1
then
AC_MSG_ERROR(
[unable to find ft2build.h, make sure FreeType2 is installed and\
 that the right -I flag is set.])
fi

AC_CHECK_HEADERS([ctr/draw.h ctr/console.h], ,[AC_MSG_ERROR(
[unable to find a ctr header, make sure ctr is installed and\
 that the right -I flag is set.] )])

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile examples/Makefile])

AC_OUTPUT

