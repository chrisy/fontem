dnl Process this file with autoconf to produce a configure script.
# vim:set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
#
# This file is distributed under the terms of the MIT License.
# See the LICENSE file at the top of this tree, or if it is missing a copy can
# be found at http://opensource.org/licenses/MIT
#

AC_PREREQ(2.67)
AC_COPYRIGHT([(c) 2014 Chris Luke])

AC_INIT([fontem], [0.1])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([aclocal])
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([subdir-objects foreign])

# Build programs
AC_PROG_MAKE_SET
AC_PROG_LN_S
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CC_C99
AM_PROG_CC_C_O
LT_INIT
AC_DISABLE_FAST_INSTALL
AC_PROG_LIBTOOL
AC_ENABLE_SHARED
AC_SUBST([LIBTOOL_DEPS])

if test x"$ac_cv_prog_cc_c99" = xno; then
        AC_MSG_ERROR([We need C99 and cannot figure out how to enable it])
fi

# Compiler flags
AX_CFLAGS_GCC_OPTION([-pipe])

# Libraries
if test -d /usr/local/lib; then
  LDFLAGS="$LDFLAGS -L/opt/local/lib"
fi
if test -d /opt/local/lib; then
  LDFLAGS="$LDFLAGS -L/opt/local/lib"
fi
AC_CHECK_LIB([popt], [poptGetContext], [], [AC_MSG_FAILURE([could not find libpopt])])
AC_CHECK_LIB([freetype], [FT_Init_FreeType], [], [AC_MSG_FAILURE([cannot find FT_Init_FreeType in libfreetype])])

# Library functions
AC_CHECK_FUNCS([memcpy strdup strndup])

# Compiler flags part deux
AX_CFLAGS_GCC_OPTION([-Wall])
AX_CFLAGS_GCC_OPTION([-W])
AX_CFLAGS_GCC_OPTION([-Wextra])

# These are warnings included in -Wextra, but we don't want all of -Wextra
AX_CFLAGS_GCC_OPTION([-Wno-unused-parameter])

# And finally, be picky
AX_CFLAGS_GCC_OPTION([-Werror])
AX_CFLAGS_GCC_OPTION([-pedantic])

# Debug mode
AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug], [configure with debugging features enabled]) ]
)
if test x"$enable_debug" = xyes; then
  CPPFLAGS="$CPPFLAGS -D_DEBUG"
  ggdb=
  if test x`uname` = xLinux; then
    ggdb=gdb
  fi
  CFLAGS="$CFLAGS -O0 -g$ggdb"
else
  CFLAGS="$CFLAGS -O2 -g"
fi

# Add our various discovered flags
CPPFLAGS="$CPPFLAGS $LIBCURL_CPPFLAGS"
CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
LIBS="$LIBS $LIBCURL $PTHREAD_LIBS"

# Headers
AC_CHECK_HEADERS([sys/types.h stdlib.h math.h string.h \
    fcntl.h sys/stat.h stdint.h])

# Definitions (typedef, struct etc)
AC_C_INLINE
case "$ac_cv_c_inline" in
    yes) cc_inline=inline;;
    no) cc_inline=;;
    *) cc_inline="$ac_cv_c_inline";;
esac
AC_SUBST([cc_inline])

# Setup our include search paths
bdir="${srcdir}"
if test x"${bdir}" = x'.' ; then
    bdir=$(pwd)
fi
#CPPFLAGS="$CPPFLAGS -I${bdir}/include"
if test -d /usr/local/include; then
    CPPFLAGS="$CPPFLAGS -I/usr/local/include"
fi
if test -d /opt/local/include; then
    CPPFLAGS="$CPPFLAGS -I/opt/local/include"
fi
if test -d /usr/include/freetype2; then
    CPPFLAGS="$CPPFLAGS -I/usr/include/freetype2"
fi
if test -d /usr/local/include/freetype2; then
    CPPFLAGS="$CPPFLAGS -I/usr/local/include/freetype2"
fi
if test -d /opt/local/include/freetype2; then
    CPPFLAGS="$CPPFLAGS -I/opt/local/include/freetype2"
fi

# Display the summary
CC=`echo $CC | sed -e 's/  / /g'`
CPPFLAGS=`echo $CPPFLAGS | sed -e 's/  / /g'`
CFLAGS=`echo $CFLAGS | sed -e 's/  / /g'`
LD=`echo $LD | sed -e 's/  / /g'`
LDFLAGS=`echo $LDFLAGS | sed -e 's/  / /g'`
LIBS=`echo $LIBS | sed -e 's/  / /g'`

echo "CC:       $CC" >&2
echo "CPPFLAGS: $CPPFLAGS" >&2
echo "CFLAGS:   $CFLAGS" >&2
echo "LD:       $LD" >&2
echo "LDFLAGS:  $LDFLAGS" >&2
echo "LIBS:     $LIBS" >&2

# Output
AC_CONFIG_HEADER([include/ac_config.h])
AC_CONFIG_FILES([Makefile
    include/Makefile
    include/cc_config.h
    src/Makefile])
AC_OUTPUT

