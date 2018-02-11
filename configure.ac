AC_INIT([libwtk-sdl2], [0.0.1], [muesli4@gmail.com])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([-Wall foreign])

# TODO what options do we need to pass?
#AM_INIT_AUTOMAKE([1.9 foreign subdir-objects])

# make sure a suitable c++ compiler is found
AC_PROG_CXX


#AC_PROG_AR
AM_PROG_AR
#m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
#m4_ifdef([AM_PROG_AR], [AM_PROG_AR], [AC_SUBST([AR], [ar])])

AM_PROG_LIBTOOL

PKG_CHECK_MODULES(sdl2, sdl2)
PKG_CHECK_MODULES(SDL2_ttf, SDL2_ttf)

# AX_BOOST_BASE([1.35.0],,[AC_MSG_ERROR([boost was not found])])

AC_CONFIG_FILES([Makefile src/Makefile])

#AM_GNU_GETTEXT([external])
#AM_GNU_GETTEXT_VERSION([0.19.7])

# generate configure script to produce Makefiles
AC_OUTPUT
