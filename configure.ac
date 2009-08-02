#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(hector, 0.0.1-dev, qiq@ucw.cz)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR([config.h.in])
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE(subdir-objects)

AC_ARG_ENABLE(debug, [  --enable-debug          turn on debugging support], use_debug=yes, use_debug=no)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for libraries.
PKG_CHECK_MODULES([LIBXML2], [libxml-2.0 > 2.5.0])
PKG_CHECK_MODULES([LOG4CXX], [liblog4cxx > 0.9.0])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h limits.h netdb.h netinet/in.h stdint.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([dup2 gethostbyname inet_ntoa memset select setlocale socket sqrt strcasecmp strdup strerror])

# Misc
dnl Check for debug options
if test "x$use_debug" = "xyes"; then
	CXXFLAGS="-ggdb -O0"
else
	CXXFLAGS="$CXXFLAGS -DNDEBUG"
fi

dnl Use -Wall if we have gcc.
if test "x$GXX" = "xyes"; then
  case " $CXXFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CXXFLAGS="$CXXFLAGS -Wall" ;;
  esac
fi

AC_CONFIG_FILES([Makefile
                src/Makefile
		src/lib/Makefile
		src/lib/server/Makefile
		src/lib/processing_chain/Makefile
		src/server/Makefile
		src/server/robot/Makefile
		src/module/Makefile
		src/bin/Makefile
		src/robot/Makefile
		src/robot/modules/Makefile
		src/robot/external-modules/Makefile
])
AC_OUTPUT
