#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_INIT([wol], [0.8])
AC_PREREQ([2.67])
AC_CONFIG_SRCDIR([wol.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIRS([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Enable "automake" to simplify creating makefiles
AM_INIT_AUTOMAKE([1.11 -Wall])
AM_PROG_AR

LT_INIT

AC_CONFIG_FILES([
	Makefile
	test/Makefile
])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset socket])

AC_OUTPUT
